<?php

/**
 * @file
 * Ad Channel include file.
 *
 * Copyright (c) 2008-2009.
 *   Jeremy Andrews <jeremy@tag1consulting.com>.
 */

/**
 * Filter advertisements not in an appropriate channel, from cache.
 */
function ad_channel_cache_filter($ads) {
  _debug_echo("ad_channel_cache: adserve_cache_filter");

  // get channel array from cache
  $channels = adserve_cache('get_cache', 'channel');
  // 0 = only display advertisements not assigned to any channel if no matching
  // ads in selected channel; 1 = always display advertisements not assigned to
  // any channel; 2 = never display advertisements not assigned to any channel
  $nochannel_display = $channels['display'];
  $valid_ads = array();
  $nochannel_fallback_ads = array();
  $nochannel_percent = array();
  // determine which channels each advertisement is assigned to
  foreach ($ads as $aid) {
    _debug_echo("ad_channel_cache: checking aid($aid)");
    if (is_array($channels['ads']) && isset($channels['ads'][$aid]) &&
        is_array($channels['ads'][$aid])) {
      foreach ($channels['ads'][$aid] as $chid) {
        $channel = $channels['channels'][$chid];
        $display_by_url = $channel->display;
        $urls = unserialize($channel->urls);
        $frontpage = adserve_variable('site_frontpage') ? adserve_variable('site_frontpage') : 'node';
        $regexp = '/^('. preg_replace(array('/(\r\n?|\n)/', '/\\\\\*/', '/(^|\|)\\\\<front\\\\>($|\|)/'), array('|', '.*', '\1'. preg_quote($frontpage, '/') .'\2'), preg_quote($urls, '/')) .')$/';
        foreach (array('url', 'alias') as $search) {
          $match = preg_match($regexp, adserve_variable($search));
          _debug_echo("ad_channel_cache: checking aid($aid) against channel($chid) path(". adserve_variable($search) .") regexp($regexp) match($match) display[$display_by_url]");
          // display ad on all except matching urls
          if ($display_by_url == 0) {
            if (empty($urls) || !$match) {
              _debug_echo("ad_channel_cache: aid($aid) is valid");
              $valid_ads[$chid][] = $aid;
              if ($nochannel_display == 1) {
                $nochannel_percent[$chid] = $channel->no_channel_percent;
                _debug_echo("ad_channel_cache: channel($chid) no_channel_percent($nochannel_percent[$chid])");
              }
              else {
                _debug_echo("ad_channel_cache: channel($chid)");
              }
              break;
            }
          }
          // display ad on matching urls
          else {
            if (!empty($urls) && $match) {
              _debug_echo("ad_channel_cache: aid($aid) is valid");
              $valid_ads[$chid][] = $aid;
              if ($nochannel_display == 1) {
                $nochannel_percent[$chid] = $channel->no_channel_percent;
                _debug_echo("ad_channel_cache: channel($chid) no_channel_percent($nochannel_percent[$chid])");
              }
              else {
                _debug_echo("ad_channel_cache: channel($chid)");
              }
              break;
            }
          }
          // no match so we didn't hit a break, ad is not valid
          _debug_echo("ad_channel_cache: aid($aid) is not valid");
        }
      }
    }
    else {
      // no channel information for ad
      _debug_echo("ad_channel_cache: aid($aid) has no channel info, nochannel_display($nochannel_display)");
      switch ($nochannel_display) {
        case 0:
          $nochannel_fallback_ads[] = $aid;
          _debug_echo("ad_channel_cache: non-channel aid($aid) is valid if no valid ads are assigned to current channel");
          break;
        case 1:
          $valid_ads[0][] = $aid;
          _debug_echo("ad_channel_cache: non-channel aid($aid) is valid");
          break;
        case 2:
          _debug_echo("ad_channel_cache: aid($aid) is not valid");
          break;
      }
    }
  }

  // Apply frequencies, applicable to all channels
  if (!empty($valid_ads) && !empty($nochannel_percent)) {
    $frequencies = array();
    foreach (array_keys($valid_ads) as $chid) {
      if ($chid) {
        if (isset($nochannel_percent[$chid])) {
          $frequencies[$chid] = $nochannel_percent[$chid];
          _debug_echo("ad_channel_cache: channel $chid has a non-channel ad frequency of ". $nochannel_percent[$chid]."%");
        }
        else {
          // by default, channels return 'non-channel ads' with a frequency
          // of 10%
          $frequencies[$chid] = 10;
          _debug_echo("ad_channel_cache: channel $chid assigned a default non-channel ad frequency of 10%");
        }
      }
      else {
        // frequency for non-channel ads is not meaningful
      }
    }
    if (!empty($frequencies)) {
      $balanced_channels = array();
      $num_channels = sizeof($valid_ads);

      foreach (array_keys($valid_ads) as $chid) {
        // if there are no non-channel ads, there is no need to balance
        if (count($valid_ads[0]) > 0) {
          if (isset($frequencies[$chid])) {
             // for a given channel, ensure the proper ratio to non-channel ads
            if ($frequencies[$chid] <=  50) { // increase occurrences of $chid
              $balanced_channels[] = 0;
              $frequency = round(100 / $frequencies[$chid]) - 1;
              _debug_echo("ad_channel_cache: adjusting ratio of channel($chid) to $frequency:1 relative non-channel ads");
              for ($i = 1; $i <= $frequency; $i++) {
                $balanced_channels[] = $chid;
              }
            }
            else { // add $chid and additional non-channel ads
              $balanced_channels[] = $chid;
              $frequency = round(100 / (100 - $frequencies[$chid])) - 1;
              _debug_echo("ad_channel_cache: adjusting ratio of channel($chid) to 1:$frequency relative non-channel ads");
              for ($i = 1; $i <= $frequency; $i++) {
                $balanced_channels[] = 0;
              }
            }
          }
        }
        else {
         _debug_echo("ad_channel_cache: NOT adjusting ratio of channel($chid) to relative non-channel ads as there are no non-channel ads");
          $balanced_channels[] = $chid;
        }
      }
      _debug_echo('ad_channel_cache: channel 0 contains all non-channel ads');
      if (adserve_variable('debug') >= 2) {
        foreach ($balanced_channels as $key => $chid) {
          _debug_echo("ad_channel_cache: channel $chid => index $key");
        }
      }
    }
    $random_channel = _select_channel_id($balanced_channels);
  }
  else if (!empty($valid_ads)) {
    foreach ($valid_ads as $chid => $ads) {
      $chids[$chid] = $chid;
    }
    shuffle($chids);
    $random_channel = array_pop($chids);
  }
  else if (empty($valid_ads) && !empty($nochannel_fallback_ads)) {
    _debug_echo("ad_channel_cache: using ads with no channel info");
    $valid_ads[0] = $nochannel_fallback_ads;
    $random_channel = 0;
  }

  $premiere = adserve_cache('get_cache', 'premiere');
  if (is_array($premiere)) {
    $premieres = array();
    foreach (array_keys($valid_ads) as $chid) {
      foreach ($valid_ads[$chid] as $aid) {
        if (in_array($aid, $premiere)) {
          _debug_echo("ad_channel_cache: aid($aid) is premiere advertisement");
          $premieres[$aid] = $aid;
        }
        else {
          _debug_echo("ad_channel_cache: aid($aid) is not a premiere advertisement");
        }
      }
    }
    if (!empty($premieres)) {
      _debug_echo("ad_channel_cache: returning premiere advertisements");
      return $premieres;
    }
  }
  _debug_echo("ad_channel_cache: returning non-premiere advertisements from randomly selected channel $random_channel");

  if (isset($valid_ads[$random_channel])) {
    return ad_channel_enforce_inventory_level($random_channel, $valid_ads[$random_channel]);
  }
}

/**
 * Randomly select a valid channel id from an array channel ids
 * @param array, valid array.
 */
function _select_channel_id($choices) {
  $selected = 0;
  if (is_array($choices)) {
    $available = sizeof($choices);
    _debug_echo("ad_channel_cache: randomly selecting from $available indexes.");
    $selected = $available > 1 ? $choices[mt_rand(0, $available - 1)] : $choices[0];
    _debug_echo("ad_channel_cache: randomly selected channel $selected.");
  }

  return $selected;
}

/*
 * Augment the selected channel with 'remnant' ads to ensure that any specified
 * inventory level is honored
 * @param int, channel id
 * @param array, valid array.
 */
function ad_channel_enforce_inventory_level($chid, $ads) {
  if ($chid > 0) {
    $channels = adserve_cache('get_cache', 'channel');
    $channel = $channels['channels'][$chid];
    $level = $channel->inventory;
    $num_ads = count($ads);
    if ($num_ads < $level) {
      _debug_echo("ad_channel_enforce_inventory_level: channel($chid) has $num_ads and needs $level");
      $remnants = array_values(adserve_cache('get_cache', 'remnant'));
      $available = count($remnants);
      if ($available > 0) {
        _debug_echo("ad_channel_enforce_inventory_level: randomly selecting from $available remnants.");
        while (count($ads) < $level) {
          shuffle($remnants);
          $selected = array_pop($remnants);
          _debug_echo("ad_channel_enforce_inventory_level: selected $selected.");
          $ads[] = $selected;
        }
      }
      else {
        _debug_echo("ad_channel_enforce_inventory_level: no remnants to choose from.");
      }
    }
    else {
      _debug_echo("ad_channel_enforce_inventory_level: channel($chid) no inventory level assigned");
    }
  }
  else {
    _debug_echo("ad_channel_enforce_inventory_level: not needed for channel($chid)");
  }
  return $ads;
}

