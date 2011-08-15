<?php
// $Id: ad_weight_probability.inc,v 1.1.4.3 2009/02/28 05:48:42 jeremy Exp $

/**
 * @file
 * A plug in for the ad.module, allowing an admin to set the probability that
 * a given advertisement will be displayed.
 *
 * Copyright (c) 2008-2009.
 *  Jeremy Andrews <jeremy@tag1consulting.com>.
 */

function ad_weight_probability_cache_filter($ads) {
  $display = array();
  if (is_array($ads)) {
    $probability = array();
    $cache = adserve_cache('get_cache', 'weight');
    foreach ($ads as $aid) {
      $probability[] = isset($cache['probability'][$aid]) ? $cache['probability'][$aid] : 0;
    }
    $gcd = ad_weight_probability_gcd($probability);
    _debug_echo("ad_weight_probability cache_filter gcd($gcd)");
    foreach ($ads as $aid) {
      $weight = (isset($cache['probability'][$aid]) && $gcd) ? $cache['probability'][$aid] / $gcd : 0;
      _debug_echo("ad_weight_probability cache_filter aid($aid) weight($weight)");
      for ($i = 1; $i <= $weight; $i++) {
        $display[] = $aid;
      }
    }
  }
  return $display;
}

/**
 * Returns the greatest common divisor of an array of integers.
 */
function ad_weight_probability_gcd($integers) {
  $gcd = array_shift($integers);

  while (!empty($integers)) {
    $gcd = _ad_weight_probability_gcd($gcd, array_shift($integers));
  }
  return $gcd;
}

/**
 * Helper function to calculate the greatest common divisor using the Euclidean
 * algorithm (http://en.wikipedia.org/wiki/Euclidean_algorithm).
 */
function _ad_weight_probability_gcd($a, $b) {
  if ($b == 0) {
    return $a;
  }
  else {
    return _ad_weight_probability_gcd($b, $a % $b);
  }
}
