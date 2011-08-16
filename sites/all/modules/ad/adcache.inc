<?php

/**
 * Wrapper for calling adserve_cache functions.
 */
function adserve_cache() {
  static $functions = array();
  $args = func_get_args();
  $function = array_shift($args);

  _debug_echo("adserve_cache function($function)");

  if (!isset($functions[$function])) {
    $cache = adserve_variable('adcache');
    $test = "ad_cache_{$cache}_$function";
    if (!function_exists($test)) {
      _debug_echo("Cache function '$test' does not exist.\n");
      $test = "adserve_cache_$function";
    }
    $functions[$function] = $test;
  }

  if (function_exists($functions[$function])) {
    _debug_memory();
    _debug_echo("Invoking cache function '". $functions[$function] ."'.");
    return call_user_func_array($functions[$function], $args);
  }
  else {
    _debug_echo("Cache function '". $functions[$function] ."' does not exist.\n");
  }
  return array();
}

/**
 * Invoke adserve cache hook, including files as necessary.
 */
function adserve_invoke_hook() {
  static $hooks = array();
  $args = func_get_args();
  $hook = array_shift($args);
  $action = array_shift($args);

  _debug_echo("adserve_invoke_hook hook($hook) action($action)");

  if (!isset($hooks[$hook])) {
    $hooks[$hook] = adserve_cache('hook', $hook);
    if (is_array($hooks[$hook]) && !empty($hooks[$hook]) &&
        is_array($hooks[$hook]['file'])) {
      // Include all necessary files.
      foreach ($hooks[$hook]['file'] as $files) {
        if (is_array($files)) {
          foreach ($files as $file) {
            $include_file = adserve_variable('root_dir') .'/'. $file;
            if (file_exists($include_file) && is_file($include_file)) {
              _debug_echo("Including file: '$include_file'.");
              include_once($include_file);
            }
            else {
              _debug_echo("Failed to include file: '$include_file'.");
            }
          }
        }
      }
    }
  }

  $return = array();
  if (is_array($hooks[$hook]) && !empty($hooks[$hook]) &&
      is_array($hooks[$hook]['function'])) {
    foreach ($hooks[$hook]['function'] as $weight => $functions) {
      foreach ($functions as $function) {
        if (function_exists($function)) {
          _debug_echo("Invoking '$function'.");
          $return[] = call_user_func_array($function, $args);
        }
        else {
          _debug_echo("Function '$function' does not exist.\n");
        }
      }
    }
  }
  else {
    $function = "adserve_hook_$hook";
    if (function_exists($function)) {
      _debug_echo("Invoking '$function'.");
      $return[] = call_user_func_array($function, $args);
    }
    else {
      _debug_echo("Function '$function' does not exist.\n");
    }
  }

  switch ($action) {
    case 'intersect':
      if (sizeof($return) == 1) {
        return $return[0];
      }
      else {
        return call_user_func_array('array_intersect', $return);
      }

    case 'merge':
      if (sizeof($return) == 1) {
        return $return[0];
      }
      else {
        $merge = array();
        foreach ($return as $array) {
          $merge += $array;
        }
        return $merge;
      }

    case 'first':
      foreach ($return as $item) {
        if (is_array($item) && !empty($item)) {
          return $item;
        }
      }
      return array();

    case 'append':
      $append = '';
      foreach ($return as $item) {
        if (!is_array($item)) {
          $append .= $item;
        }
      }
      return $append;

    default:
    case 'raw':
    default:
      return $return;
  }
}

/** Cache functions **/

/**
 * Default initialization function, fully bootstraps Drupal to gain access to
 * the database.
 */
function adserve_cache_open() {
  adserve_bootstrap();
}

/**
 * Build and return the cache.
 * TODO: It's expensive to build the cache each time we serve an ad, this should
 * be cached in the database, not in a static.
 */
function adserve_cache_get_cache($data = NULL) {
  static $cache = NULL;
  // if we don't the the cache yet, build it
  if (is_null($cache)) {
    $cache = module_invoke_all('ad_build_cache');
  }

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
 * Invoke the appropraite hook.
 */
function adserve_cache_hook($hook) {
  static $cache = NULL;
  // if we don't have the cache yet, build it
  if (is_null($cache)) {
    $external = adserve_cache('get_cache');
    $cache = adserve_cache('build_hooks', $external);
  }

  // return hook definition, if exists
  if (is_array($cache) && isset($cache["hook_$hook"]) && is_array($cache["hook_$hook"])) {
    _debug_echo("Invoking hook '$hook'.");
    return $cache["hook_$hook"];
  }
  _debug_echo("Did not find hook '$hook'.");
}

/**
 * Helper function to build hook tree.
 */
function adserve_cache_build_hooks($cache) {
  $return = array();
  if (is_array($cache)) {
    foreach ($cache as $module => $hooks) {
      // supported cache hooks
      foreach (array('hook_init', 'hook_filter', 'hook_weight', 'hook_select',
                     'hook_init_text', 'hook_exit_text',
                     'hook_increment_extra') as $hook) {
        if (isset($hooks[$hook]) && is_array($hooks[$hook])) {
          $weight = isset($hooks[$hook]['weight']) ? (int)$hooks[$hook]['weight'] : 0;
          $return[$hook]['file'][$weight][] = $hooks[$hook]['file'];
          $return[$hook]['function'][$weight][] = $hooks[$hook]['function'];
        }
      }
    }
  }
  return $return;
}

/**
 * Default function for retrieving list of ids.
 */
function adserve_cache_id($type, $id) {
  _debug_echo("adserve_cache_id: type($type) id($id)");
  switch ($type) {
    case 'nids':
      $result = db_query("SELECT aid FROM {ads} WHERE adstatus = 'active' AND aid IN(%s)", $id);
      _debug_echo("adserve_cache_id: SELECT aid FROM {ads} WHERE adstatus = 'active' AND aid IN($id)");
      break;
    case 'tids':
      $result = db_query("SELECT a.aid FROM {ads} a INNER JOIN {term_node} n ON a.aid = n.nid WHERE a.adstatus = 'active' AND n.tid IN(%s)", $id);
      _debug_echo("adserve_cache_id: SELECT a.aid FROM {ads} a INNER JOIN {term_node} n ON a.aid = n.nid WHERE a.adstatus = 'active' AND n.tid IN($id)");
      break;
    case 'default':
      $result = db_query("SELECT a.aid FROM {ads} a LEFT JOIN {term_node} n ON a.aid = n.nid WHERE a.adstatus = 'active' AND n.tid IS NULL");
      _debug_echo("adserve_cache_id: SELECT a.aid FROM {ads} a LEFT JOIN {term_node} n ON a.aid = n.nid WHERE a.adstatus = 'active' AND n.tid IS NULL");
      break;
    default:
      _debug_echo("adserve_cache_id: unsupported type '$type'.");
  }

  $ids = array();
  if (isset($result)) {
    while ($ad = db_fetch_object($result)) {
      // perform node access check
      $node = node_load($ad->aid);
      if (node_access('view', $node) !== FALSE) {
        $ids[$ad->aid] = $ad->aid;
      }
      else {
        _debug_echo("adserve_cache_id: Ad '$ad->aid' failed access check.");
      }
    }
  }
  return $ids;
}

/**
 * Support filter hooks.
 */
function adserve_hook_filter($ids, $hostid) {
  return $ids;
}

/**
 * Support weight hooks.
 */
function adserve_hook_weight($ids, $hostid) {
  return $ids;
}

/**
 * Load and display an advertisement directly from the database.
 */
function adserve_cache_display_ad($id) {
  static $modules = array();

  $ad = node_load($id);
  if (!isset($modules[$ad->adtype])) {
    $modules[$ad->adtype] = db_result(db_query("SELECT filename FROM {system} WHERE name = '%s'", "ad_$ad->adtype"));
  }
  _debug_echo("Ad type '$ad->adtype', loading module '". $modules[$ad->adtype] ."'");
  return module_invoke("ad_$ad->adtype", 'display_ad', $ad);
}

/**
 * Validate aids.
 */
function adserve_cache_validate($aids, $displayed, $hostid) {
  $valid = array();
  foreach ($aids as $aid) {
    if (!in_array($aid, $displayed)) {
      $valid[] = $aid;
    }
  }
  return $valid;
}

/**
 * Increment action directly in the database.
 */
function adserve_cache_increment($action, $aid) {
  $hostid = adserve_variable('hostid');
  _debug_echo("adserve_cache_increment action($action) aid($aid) hostid($hostid)");

  // be sure that drupal is bootstrapped
  adserve_bootstrap();

  // allow add-on modules to implement their own statistics granularity
  $extra = adserve_invoke_hook('increment_extra', 'merge', $action, $aid);
  if (is_array($extra)) {
    $extra = implode('|,|', $extra);
  }
  adserve_variable('extra', $extra);
  _debug_echo("adserve_cache_increment extra($extra)");

  // update statistics
  db_query("UPDATE {ad_statistics} SET count = count + 1 WHERE aid = %d AND action = '%s' AND date = %d AND adgroup = '%s' AND extra = '%s' AND hostid = '%s'", $aid, $action, date('YmdH'), adserve_variable('group'), $extra, $hostid);
  // if column doesn't already exist, add it
  if (!db_affected_rows()) {
    db_query("INSERT INTO {ad_statistics} (aid, date, action, adgroup, extra, hostid, count) VALUES(%d, %d, '%s', '%s', '%s', '%s', 1)", $aid, date('YmdH'), $action, adserve_variable('group'), $extra, $hostid);
    if (!db_affected_rows()) {
      // we lost a race to add it, increment it
      db_query("UPDATE {ad_statistics} SET count = count + 1 WHERE aid = %d AND action = '%s' AND date = %d AND adgroup = '%s' AND extra = '%s' AND hostid = '%s'", $aid, $action, date('YmdH'), adserve_variable('group'), $extra, $hostid);
    }
  }

  if ($action == 'view') {
    $ad = db_fetch_object(db_query('SELECT maxviews, activated FROM {ads} WHERE aid = %d', $aid));
    // See if we need to perform additional queries.
    if ($ad->maxviews) {
      $views = (int)db_result(db_query("SELECT SUM(count) FROM {ad_statistics} WHERE aid = %d AND action = 'view' AND date >= %d", $aid, date('YmdH', $ad->activated)));
      if ($views >= $ad->maxviews) {
        db_query("UPDATE {ads} SET adstatus = 'expired', autoexpire = 0, autoexpired = %d, expired = %d WHERE aid = %d", time(), time(), $aid);
        ad_statistics_increment($aid, 'autoexpired');
        ad_statistics_increment($aid, 'expired');
      }
    }
  }
}

/**
 * Randomly select advertisements.
 * @param array, valid ad ids.
 * @param integer, how many advertisements to select
 * @param string, the hostid
 */
function adserve_hook_select($ids, $quantity = 1, $hostid = '') {
  $select = 0;
  $selected = array();
  if (is_array($ids)) {
    $ads = $ids;
    foreach ($ids as $key => $value) {
      $available = sizeof($ads);
      $select++;
      _debug_echo("Randomly selecting ad $select of $quantity.");
      $id = 0;
      if ($id == 0) {
        $id = $available > 1 ? $ads[mt_rand(0, $available - 1)] : $ads[0];
        _debug_echo("Randomly selected ID: $id.");
        $selected[] = $id;
        // strip away advertisments that have already been selected
        $ads = adserve_cache('validate', $ads, array($id), $hostid);
      }
      if (($quantity == $select) || !count($ads)) {
        // we have selected the right number of advertisements
        break;
      }
    }
  }
  if ($select < $quantity) {
    _debug_echo('No more advertisements available.');
  }
  return $selected;
}
/**
 * Default wrapper function for displaying advertisements.  This generally
 * is not replaced by ad caches modules.
 */
function adserve_cache_get_ad_ids() {
  static $displayed_count = 0;
  _debug_echo('Entering default adserve_display.');

  // open the cache
  adserve_cache('open');

  $hostid = adserve_variable('hostid') ? adserve_variable('hostid') : 'none';
  _debug_echo("Hostid: '$hostid'.");

  // invoke hook_init
  $init = adserve_invoke_hook('init', 'first', $hostid);

  // start with list of advertisements provided externally
  if (is_array($init) && !empty($init)) {
    _debug_echo('Initialized externally.');
    $quantity = $init['quantity'];
    $id = $init['id'];
    $aids = explode(',', $id);
    $type = $init['type'];
  }
  else {
    // build list of ad ids to choose from
    $quantity = adserve_variable('quantity');
    // use list for specific host
    if ($ids = adserve_cache('id', 'host', NULL, $hostid)) {
      $id = implode(',', $ids);
      $type = 'host';
    }
    // use list of node ids
    else if ($id = adserve_variable('nids')) {
      $type = 'nids';
      adserve_variable('group', "n$id");
    }
    // use list of group ids
    else if ($id = adserve_variable('tids')) {
      $type = 'tids';
      adserve_variable('group', "t$id");
    }
    // use list without group ids
    else {
      $id = 0;
      $type = 'default';
      adserve_variable('group', "$id");
    }
    _debug_echo("Searching $type: $id");
    $aids = adserve_cache('id', $type, $id, $hostid);
  }

  // prepare to select advertisements
  $number_of_ads = sizeof($aids);
  _debug_echo("Total ads: '$number_of_ads'.");

  $displayed = adserve_variable("$type-displayed");
  if (!is_array($displayed)) {
    $displayed = array();
  }
  _debug_echo('Already displayed: '. sizeof($displayed));

  // validate available advertisements
  $aids = adserve_cache('validate', $aids, $displayed, $hostid);
  $number_of_ads = sizeof($aids);
  _debug_echo("Validated ads: '$number_of_ads'.");

  // filter advertisements
  $aids = adserve_invoke_hook('filter', 'intersect', $aids, $hostid);
  $number_of_ads = sizeof($aids);
  _debug_echo("Filtered ads: '$number_of_ads'.");


  // apply weight to advertisements
  $aids = adserve_invoke_hook('weight', 'first', $aids, $hostid);
  $number_of_ads = sizeof($aids);
  _debug_echo("Weighted ads: '$number_of_ads'.");

  // select advertisements
  $aids = adserve_invoke_hook('select', 'first', $aids, $quantity, $hostid);
  $number_of_ads = sizeof($aids);
  _debug_echo("Selected ads: '$number_of_ads'.");

  // track which advertisements have been "displayed"
  adserve_variable("$type-displayed", array_merge($aids, $displayed));

  return $aids;
}

/**
 * Default function for displaying advertisements.  This is not generally
 * replaced by ad cache modules.
 */
function adserve_cache_display($ids) {
  $output = '';
  $ads = 0;
  foreach ($ids as $id) {
    $ad = adserve_cache('display_ad', $id);
    _debug_echo('ad: '. htmlentities($ad));
    // if displaying multiple ads, separate each with a div
    if ($output) {
      $group = adserve_variable('group');
      $output .= "<div class=\"advertisement-space\" id=\"space-$group-$ads\"></div>";
    }
    // display advertisement
    $output .= $ad;
    // increment counters
    if (adserve_variable('ad_display') == 'raw') {
      $output .= ad_display_image($id);
    }
    else {
      adserve_cache('increment', 'view', $id);
    }
    $ads++;
  }

  if (empty($ids)) {
    adserve_variable('error', TRUE);
    $output = 'No active ads were found in '. adserve_variable('group');
    adserve_cache('increment', 'count', NULL);
  }

  // close/update cache, if necessary
  adserve_cache('close');

  // update the dynamic portion of the output
  $params = array();
  $group = adserve_variable('group');
  $replace = "/$group";
  if ($hostid = adserve_variable('hostid')) {
    $params[] = "hostid=$hostid";
  }
  if ($url = htmlentities(adserve_variable('url'))) {
    $params[] = "url=$url";
  }
  if ($extra = adserve_variable('extra')) {
    $params[] = "extra=$extra";
  }
  if (!empty($params)) {
    $replace .= '?'. implode('&', $params);
  }
  $output = preg_replace('&/@HOSTID___&', $replace, $output);

  // there was an error, hide the output in comments
  if (adserve_variable('error')) {
    $output = "<!-- $output -->";
  }

  // allow custom text to be displayed before and after advertisement
  $init_text = adserve_invoke_hook('init_text', 'append');
  $exit_text = adserve_invoke_hook('exit_text', 'append');
  $output = $init_text . $output . $exit_text;

  _debug_memory();

  // TODO: turn all of these into hooks
  switch (adserve_variable('ad_display')) {
    case 'javascript':
    default:
      $output = str_replace(array("\r", "\n", "<", ">", "&"),
                            array('\r', '\n', '\x3c', '\x3e', '\x26'),
                            addslashes($output));
      if (!adserve_variable('debug')) {
        // Tell the web browser not to cache this script so the ad refreshes
        // each time the page is viewed.
        // Expires in the past:
        header('Expires: Mon, 26 Jul 1997 05:00:00 GMT');
        // Last load:
        header('Last-Modified: '. gmdate('D, d M Y H:i:s') .' GMT');
        // HTTP 1.1:
        header('Cache-Control: no-store, no-cache, must-revalidate');
        header('Cache-Control: post-check=0, pre-check=0', false);
        // HTTP 1.0:
        header('Pragma: no-cache');
        // Output is a JavaScript:
        header('Content-Type: application/x-javascript; charset=utf-8');
      }
      print "document.write('$output');";
      exit(0);
    case 'iframe':
    case 'jquery':
      if (!adserve_variable('debug')) {
        // Tell the web browser not to cache this frame so the ad refreshes
        // each time the page is viewed.

        // Expires in the past:
        header('Expires: Mon, 26 Jul 1997 05:00:00 GMT');
        // Last load:
        header('Last-Modified: '. gmdate('D, d M Y H:i:s') .' GMT');
        // HTTP 1.1:
        header('Cache-Control: no-store, no-cache, must-revalidate');
        header('Cache-Control: post-check=0, pre-check=0', false);
        // HTTP 1.0:
        header('Pragma: no-cache');
      }
      else {
        _debug_echo('Output: '. htmlentities($output));
      }
      print "$output";
      exit(0);
    case 'raw':
      _debug_echo('Output: '. htmlentities($output));
      chdir(adserve_variable('ad_dir'));
      return $output;

  }

  _debug_echo('Output: '. htmlentities($output));
  return $output;
}

