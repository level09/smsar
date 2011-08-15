<?php
// $Id: imageserve.inc,v 1.1.2.8.2.2.2.2 2009/07/09 21:21:38 jeremy Exp $

/**
 * @file
 * Image serving lib.
 *
 * Copyright (c) 2008-2009.
 *   Jeremy Andrews <jeremy@tag1consulting.com>.
 */

/**
 * Generate a tiny image with GD, used to count when an ad has been displayed
 * on a cached page.
 */
function adserve_counter_image() {
  adserve_variable('variable_load');
  adserve_bootstrap(0);

  if (adserve_variable('aid')) {
    $aid = adserve_variable('aid');
  }
  if (isset($aid) && $aid) {
    _debug_echo("adserve_counter_image: increment 'view' counter for aid: $aid");
    adserve_cache('increment', 'view', $aid);
  }
  else {
    _debug_echo("adserve_counter_image: increment 'count' counter for no aid");
    adserve_cache('increment', 'count', NULL);
  }

  if (function_exists('imagecreate')) {
    $image = imagecreate(1, 1);
    // Tell the web browser not to cache this image so we register a count each
    // time the page is viewed.
    // Expires in the past:
    header('Expires: Mon, 26 Jul 1997 05:00:00 GMT');
    // Last loud:
    header('Last-Modified: '. gmdate('D, d M Y H:i:s') .' GMT');
    // HTTP 1.1:
    header('Cache-Control: no-store, no-cache, must-revalidate');
    header('Cache-Control: post-check=0, pre-check=0', FALSE);
    // HTTP 1.0:
    header('Pragma: no-cache');
  }
  else {
    // GD not installed, report error and exit.
    exit();
  }

  if (function_exists('imagejpeg')) {
    header("Content-type: image/jpeg");
    imagejpeg($image);
  }
  else if (function_exists('imagepng')) {
    header("Content-type: image/png");
    imagepng($image);
  }
  else if (function_exists('imagegif')) {
    header("Content-type: image/gif");
    imagegif($image);
  }
  imagedestroy($image);
  exit(0);
}
