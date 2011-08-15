<?php
// $Id: ad_weight_percent.inc,v 1.1.2.2.2.2 2009/02/16 17:06:50 jeremy Exp $

/**
 * @file
 * A plug in for the ad.module, providing a percentage based weighting mechanism
 * for the random selection of ads.
 *
 * Copyright (c) 2007-2009.
 *  Jeremy Andrews <jeremy@tag1consulting.com>.
 */

function ad_weight_percent_select_ad($ads, $quantity, $invalid) {
  $tid = NULL;
  if (adserve_variable('tids')) {
    $id = adserve_variable('tids');
    $tids = explode(',', $id);
    // A group can only be comprised of one tid.  If multiple tids are defined,
    // then we're not working with a group.
    if (sizeof($tids) == 1) {
      $tid = $tids[0];
    }
  }
  else if (!adserve_variable('nids')) {
    // The default group.
    $tid = 0;
  }

  // Only do something if we're currently processing a specific ad group.
  if ($tid !== NULL) {
    // TODO: The percentage information needs to be stored in the file cache,
    // as obviously we don't want to bootstrap the database layer and perform
    // a query at this point.
  }
}
