<?php
// $Id: ad_cache_file.inc,v 1.1.4.23.2.5.2.6 2009/05/07 13:52:13 jeremy Exp $

/**
 * @file
 * A plug in for the ad.module, providing a file cache mechanism for improved
 * performance when displaying ads.
 *
 * Copyright (c) 2007-2009.
 *  Jeremy Andrews <jeremy@tag1consulting.com>.
 */

/**
 * Initialization function.  Loads cache from file into memory.
 */
function ad_cache_file_open() {
  _debug_echo('File cache: open');
  $cache_file = ad_cache_file_get_lock();
  if ($cache_file) {
    // Read cache from disk into memory.
    $cache = unserialize(fread(adserve_variable('fd'), filesize($cache_file)));
    // Store cache in a static variable for re-use by other functions.
    ad_cache_file_cache($cache);
  }

  if (adserve_variable('debug')) {
    $last_sync = $cache['last_sync'];
    $lifetime = $cache['lifetime'];
    $time = time();
    echo "File cache: last sync: $last_sync<br />\n";
    echo "File cache: current time: $time<br />\n";
    if (adserve_variable('ad_cache_file_rebuild_realtime')) {
      if ($last_sync < $time - $lifetime) {
        echo "File cache: will resync cache now.<br />\n";
      }
      else {
        $seconds = $last_sync - $time + $lifetime;
        echo "File cache: will resync cache in $seconds seconds.<br/>\n";
      }
    }
    else {
      if ($last_sync < $time - $lifetime) {
        echo "File cache: cron will resync cache the next time it runs.<br />\n";
      }
      else {
        $seconds = $last_sync - $time + $lifetime;
        echo "File cache: cron will resync cache after $seconds seconds.<br/>\n";
      }
    }
  }
}

/**
 * Return hook definition.
 */
function ad_cache_file_hook($hook) {
  $cache = ad_cache_file_cache();
  if (isset($cache["hook_$hook"]) && is_array($cache["hook_$hook"])) {
    return $cache["hook_$hook"];
  }
  else {
    _debug_echo("File cache: hook '$hook' not found.");
  }
  return array();
}

/**
 * Return the cache structure.
 */
function ad_cache_file_get_cache($data = NULL) {
  $cache = ad_cache_file_cache();
  if ($data) {
    if (isset($cache[$data])) {
      return $cache[$data];
    }
    else {
      return NULL;
    }
  }
  return $cache;
}

/**
 * Return an array of aids to choose an advertisement from.
 */
function ad_cache_file_id($type, $id, $hostid) {
  $cache = ad_cache_file_cache();
  _debug_echo("File cache: ad_cache_file_id type($type) id($id) hostid($hostid)");
  switch ($type) {
    case 'host':
      if (isset($cache['hostid'][$hostid]['aids'])) {
        return ($cache['hostid'][$hostid]['aids']);
      }
      break;

    case 'nids':
      return explode(',', $id);

    case 'tids':
      if (!isset($cache['tids'][$id])) {
        $cache['tids'][$id] = array();
        $tids = explode(',', $id);
        foreach ($tids as $tid) {
          if (is_array($cache['tid'][$tid]['aid'])) {
            $cache['tids'][$id] += $cache['tid'][$tid]['aid'];
          }
        }
        // rebuild keys from 0, cache for re-use on next ad display
        $cache['tids'][$id] = array_values($cache['tids'][$id]);
        // update cache
        ad_cache_file_cache($cache);
      }
      return $cache['tids'][$id];

    case 'default':
      return $cache['tid'][0]['aid'];

    default:
      _debug_echo("File cache: unkown id type '$type'.");
      break;
  }
}

/**
 * Validate advertisement ids, filtering those that shouldn't be displayed.
 */
function ad_cache_file_validate($aids, $displayed, $hostid) {
  $valid = array();
  if (is_array($aids)) {
    $cache = ad_cache_file_cache();
    foreach ($aids as $aid) {
      // Only include aids that are in our cache, others are not valid in our
      // context.  Also, don't display the same ad twice.
      if (isset($cache['ad'][$aid]) && !in_array($aid, $displayed)) {
        $valid[] = $aid;
      }
    }
  }
  if (adserve_variable('debug')) {
    $count = sizeof($valid);
    _debug_echo("File cache: found $count valid advertisements.");
  }
  return $valid;
}

/**
 * Display a given advertisement.
 */
function ad_cache_file_display_ad($id) {
  $cache = ad_cache_file_cache();
  $hostid = adserve_variable('hostid') ? adserve_variable('hostid') : 'none';
  if ($hostid != 'none' && !isset($cache['hostid'][$hostid])) {
    _debug_echo("File cache: invalid hostid: '$hostid'.");
    $output = 'You do not have permission to display ads.';
  }
  else {
    if (is_array($cache) && is_array($cache['ad']) &&
        is_array($cache['ad'][$id])) {
      return ($cache['ad'][$id]['display']);
    }
  }
}

/**
 * Increment an advertisement counter.
 */
function ad_cache_file_increment($action, $aid) {
  $group = adserve_variable('group');
  $hostid = adserve_variable('hostid') ? adserve_variable('hostid') : 'none';
  $cache = ad_cache_file_cache();
  $timestamp = date('YmdH');

  $extra = adserve_invoke_hook('increment_extra', 'merge', $action, $aid);
  if (is_array($extra) && !empty($extra)) {
    $extra = implode('|,|', $extra);
  }
  else if (empty($extra)) {
    $extra = '';
  }
  adserve_variable('extra', $extra);

  // increment counter
  if (is_array($cache['ad'][$aid]) &&
      isset($cache['ad'][$aid]['counts']) &&
      isset($cache['ad'][$aid]['counts'][$group]) &&
      is_array($cache['ad'][$aid]['counts'][$group]) &&
      is_array($cache['ad'][$aid]['counts'][$group][$extra]) &&
      is_array($cache['ad'][$aid]['counts'][$group][$extra][$hostid]) &&
      is_array($cache['ad'][$aid]['counts'][$group][$extra][$hostid][$action]) &&
      is_array($cache['ad'][$aid]['counts'][$group][$extra][$hostid][$action]) &&
      isset($cache['ad'][$aid]['counts'][$group][$extra][$hostid][$action][$timestamp])) {
    $cache['ad'][$aid]['counts'][$group][$extra][$hostid][$action][$timestamp]++;
  }
  else {
    $cache['ad'][$aid]['counts'][$group][$extra][$hostid][$action][$timestamp] = 1;
  }
  _debug_echo("File cache: aid($aid) group($group) extra($extra) hostid($hostid) action($action) timestamp($timestamp) count: ". $cache['ad'][$aid]['counts'][$group][$extra][$hostid][$action][$timestamp]);

  // update the cache in memory
  ad_cache_file_cache($cache);
}

/**
 * Close the cache file and write updated cache to disk.
 */
function ad_cache_file_close() {
  static $written = FALSE;

  // prevent accidentally writing one version of the cache to a different file
  if ($written) {
    _debug_echo('File cache: unable to write cache file, already closed.');
    return(1);
  }
  $written = TRUE;

  $cache_file = ad_cache_file_get_lock();
  _debug_echo("File cache: writing cache back to file '$cache_file'.");

  // gather data to determine if we should flush the cache
  $cache = ad_cache_file_cache();
  $last_sync = $cache['last_sync'];
  $lifetime = $cache['lifetime'];

  // serialize the array to write it to disk
  $cache = serialize($cache);

  // write updated cache back to file and release the lock
  if (rewind(adserve_variable('fd')) == TRUE) {
    if (ftruncate(adserve_variable('fd'), 0) == TRUE) {
      $bytes = fwrite(adserve_variable('fd'), $cache, strlen($cache));
      if ($bytes) {
        _debug_echo("File cache: wrote $bytes bytes to '$cache_file'.");
      }
      else {
        _debug_echo("File cache: failed to write to '$cache_file'.");
      }
      flock(adserve_variable('fd'), LOCK_UN);
      if (fclose(adserve_variable('fd')) == TRUE) {
        _debug_echo("File cache: successfully closed '$cache_file'.");
      }
      else {
        _debug_echo("File cache: failed to close '$cache_file'.");
      }
    }
    else {
      _debug_echo("File cache: failed to ftruncate file '$cache_file'.");
    }
  }
  else {
    _debug_echo("File cache: failed to rewind file '$cache_file'.");
  }
  adserve_variable('fd', '');

  if (adserve_variable('ad_cache_file_rebuild_realtime')) {
    // every $lifetime seconds we flush the cache to the database
    $time = time();
    if ($last_sync < time() - $lifetime) {
      ad_cache_file_rebuild();
    }
  }
}

/**
 * Try and obtain a lock on one of the available cache files.  If we already
 * have a lock, simply return the filename.
 */
function ad_cache_file_get_lock() {
  static $lock = FALSE;
  static $cache_file = '';

  if ($lock) {
    _debug_echo('File cache: already have lock.');
    return $cache_file;
  }

  // We'll loop through all possible cache files until we obtain an exclusive
  // lock.
  for ($i = 1; $i <= adserve_variable('files'); $i++) {
    // Prefix the filename with a '.' to hide it on Unix systems.
    $cache_file = adserve_variable('root_dir') .'/'. adserve_variable('path') .'/.'. $i .'.ad.cache';

    if (adserve_variable('debug')) {
      echo "Trying cache_file '$cache_file'.<br />\n";
    }

    if (!$fd = @fopen($cache_file, 'r+')) {
      if (adserve_variable('debug')) {
        echo "Failed to open cache_file '$cache_file'.<br />\n";
      }
      // We failed to open the cache file, try the next one.
      continue;
    }
    if ($i < adserve_variable('files')) {
      // This isn't the last available cache file so we'll use a
      // non-blocking lock for best performance.  If we fail to lock this
      // cache file, we'll quickly move on to the next until we find an
      // available one.
      if (!flock($fd, LOCK_EX|LOCK_NB)) {
        if (adserve_variable('debug')) {
          echo "Failed to obtain non-blocking lock.<br />\n";
        }
        // We failed to obtain an exclusive lock, close the file and try the
        // next one.
        @fclose($fd);
        continue;
      }
      if (adserve_variable('debug')) {
        echo "Obtained lock.<br />\n";
        $stat = fstat($fd);
        echo 'File size: '. $stat['size'] .'<br />';
      }
      $lock = TRUE;
      break;
    }
    else {
      // This is the last available cache file, we'll use a blocking lock
      // as we have to wait until we have exclusive write permissions.
      if (!flock($fd, LOCK_EX)) {
        if (adserve_variable('debug')) {
          echo "Failed to obtain blocking lock.<br />\n";
        }
        // A blocking exclusive lock shouldn't ever fail, so something has
        // gone very wrong.  Perhaps the file was deleted out from under us?
        @fclose($fd);
        continue;
      }
      if (adserve_variable('debug')) {
        echo "Obtained lock on final cache file.<br />\n";
      }
      $lock = TRUE;
      break;
    }
  }
  if ($lock) {
    adserve_variable('fd', $fd);
    return $cache_file;
  }
  else {
    return NULL;
  }
}

/**
 *  Additional variables required by the filecache.
 */
function ad_cache_file_variables() {
  // paths are comprised of alphanumerics, underscores, dashes, periods and
  // slashes.
  $variables = array();
  $variables['path'] = isset($_GET['p']) ? preg_replace('/[^_\-\.\/\\0-9a-zA-Z]/', '', $_GET['p']) : 'files';
  // files is an integer.
  $variables['files'] = isset($_GET['f']) ? (int)$_GET['f'] : 1;
  if ($variables['files'] > 15) {
      echo "Invalid value 'f=". $variables['files'] ."', exiting.<br />\n";
      exit();
  }
  return $variables;
}

/**
 * Bootstrap drupal, then run ad_cache_file_build() from ad.module which will
 * rebuild all cache files.
 */
function ad_cache_file_rebuild() {
  adserve_bootstrap();
  if (function_exists('ad_cache_file_build')) {
    ad_cache_file_build();
    return 0;
  }
  else {
    return -1;
  }
}

/**
 * Keep a copy of the cache in a static.
 */
function ad_cache_file_cache($update = array()) {
  static $cache = array();

  if (!empty($update)) {
    $cache = $update;
  }
  return $cache;
}

