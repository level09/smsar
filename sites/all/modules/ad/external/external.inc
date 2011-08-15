<?php
// $Id: external.inc,v 1.1.2.1.2.2 2009/02/16 17:06:48 jeremy Exp $

/**
 * @file
 * Loads external ad.
 *
 * Copyright (c) 2006-2009.
 *   Jeremy Andrews <jeremy@tag1consulting.com>.
 */

adserve_variable('ad_dir', getcwd() .'/');

// Attempt to utilize the file cache, if enabled.
if (function_exists('ad_cache_file_get_lock')) {

  // Find the top level Drupal directory so we can access the cache files.
  for ($i = 1; $i <= 20; $i++) {
    if (file_exists('./includes/bootstrap.inc')) {
      break;
    }
    chdir('..');
  }
  adserve_variable('root_dir', getcwd());

  $cache_file = ad_cache_file_get_lock();
  if ($cache_file) {
    $cache = unserialize(fread(adserve_variable('fd'), filesize($cache_file)));
    if (isset($cache['ad_external'])) {
      $aid = adserve_variable('nids');
      if ($aid && isset($cache['ad_external'][$aid]['contents'])) {
        print $cache['ad_external'][$aid]['contents'];
        exit(0);
      }
    }
  }
}
