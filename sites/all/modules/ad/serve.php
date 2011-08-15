<?php
// $Id: serve.php,v 1.1.2.2.2.1.2.1 2009/02/23 22:39:02 jeremy Exp $

/**
 * @file
 * Serve advertisements.
 *
 * Copyright (c) 2005-2009.
 *   Jeremy Andrews <jeremy@tag1consulting.com>.
 */

// Provide adserve functions to external plugins.
require_once('adserve.inc');
require_once('adcache.inc');

if (isset($_GET['o'])) {
  $output = preg_replace('/[^a-zA-Z0-9_-]/', '', $_GET['o']);
  if ($output == 'image') {
    require_once('imageserve.inc');
    adserve_counter_image();
    exit(0);
  }
  else if ($output) {
    // TODO: Document how this hook allows external modules to use serve.php
    $files = array(
      "$output.inc", // search for file in main ad directory
      "$output/$output.inc", // search for file in subdirectory
      "../$output/$output.inc", // search for file in higher subdirectory
    );
    foreach ($files as $file) {
      $function = $output .'_serve';
      if (file_exists($file)) {
        require_once("$file");
        if (function_exists($function)) {
          $function();
          exit(0);
        }
      }
    }
  }
}

// Default action.
adserve_ad();
