<?php
// $Id: ad_token.inc,v 1.1.2.1 2009/03/29 20:17:20 jeremy Exp $

/**
 * @file
 * Implementations of token module hooks for the Advertisement module.
 *
 * @ingroup token
 */

/**
 * Implementation of hook_token_values().
 */
function ad_token_values($type, $object = NULL, $options = array()) {
  $values = array();
  switch ($type) {
    case 'ad':
      if (isset($object)) {
        $node = $object;
        $notifications = module_invoke_all('adnotifyapi', 'register');
        module_load_include('pages.inc', 'ad');

        $values['aid']           = $node->nid;
        $values['title']         = $node->title;
        $values['description']   = $node->body;
        $values['log_message']   = $node->log;
        $values['type']          = $node->adtype;
        $values['status']        = $node->adstatus;
        $values['url']           = url('node/'. $node->nid, array('absolute' => TRUE));
        $values['redirect']      = url($node->redirect, array('absolute' => TRUE));
        $values['comments']      = $node->comment_count;

        if (isset($node->notification)) {
          $values['event']       = $node->notification->event;
          $values['frequency']   = t(strtolower($notifications[$node->notification->event]), array('@when' => format_interval($node->notification->delay)));
        }

        $values['created_small'] = format_date($node->created, 'small');
        $values['created_medium'] = format_date($node->created, 'medium');
        $values['created_large'] = format_date($node->created, 'large');
        $values['activated_small'] = $node->activated ? format_date($node->activated, 'small') : t('never');
        $values['activated_medium'] = $node->activated ? format_date($node->activated, 'medium') : t('never');
        $values['activated_large'] = $node->activated ? format_date($node->activated, 'large') : t('never');
        $values['expired_small'] = $node->expired ? format_date($node->expired, 'small') : t('never');
        $values['expired_medium'] = $node->expired ? format_date($node->expired, 'medium') : t('never');
        $values['expired_large'] = $node->expired ? format_date($node->expired, 'large') : t('never');
        $values['autoactivate_small'] = $node->autoactivate ? format_date($node->autoactivate, 'small') : t('never');
        $values['autoactivate_medium'] = $node->autoactivate ? format_date($node->autoactivate, 'medium') : t('never');
        $values['autoactivate_large'] = $node->autoactivate ? format_date($node->autoactivate, 'large') : t('never');
        $values['autoexpire_small'] = $node->autoexpire ? format_date($node->autoexpire, 'small') : t('never');
        $values['autoexpire_medium'] = $node->autoexpire ? format_date($node->autoexpire, 'medium') : t('never');
        $values['autoexpire_large'] = $node->autoexpire ? format_date($node->autoexpire, 'large') : t('never');

        $statistics = ad_statistics($node->nid);
        // maximums
        $values['max_impressions'] = $node->maxviews;
        $values['max_clicks'] = $node->maxclicks;
        // global statistics
        $values['global_impressions'] = isset($statistics['global']) && !empty($statistics['global']) ? $statistics['global']['views'] : 0;
        $values['global_clicks'] = isset($statistics['global']) && !empty($statistics['global']) ? $statistics['global']['clicks'] : 0;
        // last year statistics
        $values['last_year_impressions'] = isset($statistics['last_year']) && !empty($statistics['last_year']) ? $statistics['last_year']['views'] : 0;
        $values['last_year_clicks'] = isset($statistics['last_year']) && !empty($statistics['last_year']) ? $statistics['last_year']['clicks'] : 0;
        // this year statistics
        $values['this_year_impressions'] = isset($statistics['this_year']) && !empty($statistics['this_year']) ? $statistics['this_year']['views'] : 0;
        $values['this_year_clicks'] = isset($statistics['this_year']) && !empty($statistics['this_year']) ? $statistics['this_year']['clicks'] : 0;
        // last month statistics
        $values['last_month_impressions'] = isset($statistics['last_month']) && !empty($statistics['last_month']) ? $statistics['last_month']['views'] : 0;
        $values['last_month_clicks'] = isset($statistics['last_month']) && !empty($statistics['last_month']) ? $statistics['last_month']['clicks'] : 0;
        // this month statistics
        $values['this_month_impressions'] = isset($statistics['this_month']) && !empty($statistics['this_month']) ? $statistics['this_month']['views'] : 0;
        $values['this_month_clicks'] = isset($statistics['this_month']) && !empty($statistics['this_month']) ? $statistics['this_month']['clicks'] : 0;
        // yesterday statistics
        $values['yesterday_impressions'] = isset($statistics['yesterday']) && !empty($statistics['yesterday']) ? $statistics['yesterday']['views'] : 0;
        $values['yesterday_clicks'] = isset($statistics['yesterday']) && !empty($statistics['yesterday']) ? $statistics['yesterday']['clicks'] : 0;
        // today statistics
        $values['today_impressions'] = isset($statistics['today']) && !empty($statistics['today']) ? $statistics['today']['views'] : 0;
        $values['today_clicks'] = isset($statistics['today']) && !empty($statistics['today']) ? $statistics['today']['clicks'] : 0;
        // last hour statistics
        $values['last_hour_impressions'] = isset($statistics['last_hour']) && !empty($statistics['last_hour']) ? $statistics['last_hour']['views'] : 0;
        $values['last_hour_clicks'] = isset($statistics['last_hour']) && !empty($statistics['last_hour']) ? $statistics['last_hour']['clicks'] : 0;
        // this hour statistics
        $values['this_hour_impressions'] = isset($statistics['this_hour']) && !empty($statistics['this_hour']) ? $statistics['this_hour']['views'] : 0;
        $values['this_hour_clicks'] = isset($statistics['this_hour']) && !empty($statistics['this_hour']) ? $statistics['this_hour']['clicks'] : 0;
      }
      break;
    case 'ad_owner':
      if (isset($object) && is_object($object)) {
        $owner = $object;
        $values['owner_name']    = $owner->name;
        $values['owner_mail']    = $owner->mail;
        $values['owner_uid']     = $owner->uid;
      }
      break;
  }
  return $values;
}

/**
 * Implementation of hook_token_list().
 */
function ad_token_list($type = 'all') {
  if ($type == 'ad' || $type == 'all') {
    $tokens['ad']['aid']           = t('The ID of the advertisement.');
    $tokens['ad']['type']          = t('The type of ad.');
    $tokens['ad']['status']        = t('The status of the ad.');
    $tokens['ad']['url']           = t('The url of the advertisement.');
    $tokens['ad']['redirect']      = t('The redirection url of the advertisement.');
    $tokens['ad']['event']         = t('The type of event that has triggered this notification.');
    $tokens['ad']['frequency']     = t('A complete sentence describing the frequency this notification will be sent.');
    $tokens['ad']['title']         = t('The title of the advertisement.');
    $tokens['ad']['comments']      = t('The number of comments attached to the advertisement.');

    $tokens['ad']['created_small']        = t('"Small" date format of when the advertisement was created.');
    $tokens['ad']['created_medium']       = t('"Medium" date format of when the advertisement was created.');
    $tokens['ad']['created_large']        = t('"Large" date format of when the advertisement was created.');
    $tokens['ad']['activated_small']      = t('"Small" date format when the advertisement was activated.');
    $tokens['ad']['activated_medium']     = t('"Medium" date format of when the advertisement was activated.');
    $tokens['ad']['activated_large']      = t('"Large" date format of when the advertisement was activated.');
    $tokens['ad']['expired_small']        = t('"Small" date format of when the advertisement was expired.');
    $tokens['ad']['expired_medium']       = t('"Medium" date format of when the advertisement was expired.');
    $tokens['ad']['expired_large']        = t('"Large" date format of when the advertisement was expired.');
    $tokens['ad']['autoactivate_small']   = t('"Small" date format of when the advertisement was automatically activated.');
    $tokens['ad']['autoactivate_medium']  = t('"Medium" date format of when the advertisement was automatically activated.');
    $tokens['ad']['autoactivate_large']   = t('"Large" date format of when the advertisement was automatically activated.');
    $tokens['ad']['autoexpire_small']     = t('"Small" date format of when the advertisement was automatically expired.');
    $tokens['ad']['autoexpire_medium']    = t('"Medium" date format of when the advertisement was automatically expired.');
    $tokens['ad']['autoexpire_large']     = t('"Large" date format of when the advertisement was automatically expired.');

    $tokens['ad']['max_impressions']          = t('The maximum number of times this advertisement is allowed to be viewed.');
    $tokens['ad']['max_clicks']         = t('The maximum number of times this advertisement is allowed to be clicked.');
    $tokens['ad']['global_impressions']      = t('All time impression statistics');
    $tokens['ad']['global_clicks']     = t('All time click statistics.');
    $tokens['ad']['last_year_impressions'] = t('Ad impressions last year.');
    $tokens['ad']['last_year_clicks']  = t('Ad clicks last year.');
    $tokens['ad']['this_year_impressions']   = t('Ad impressions this year.');
    $tokens['ad']['this_year_clicks']  = t('Ad clicks this year.');
    $tokens['ad']['last_month_impressions']  = t('Ad impressions last month.');
    $tokens['ad']['last_month_clicks'] = t('Ad clicks this month.');
    $tokens['ad']['this_month_impressions']  = t('Ad impressions this month.');
    $tokens['ad']['this_month_clicks'] = t('Ad clicks this month.');
    $tokens['ad']['yesterday_impressions']   = t('Ad impressions yesterday.');
    $tokens['ad']['yesterday_clicks']  = t('Ad clicks yesterday.');
    $tokens['ad']['today_impressions']       = t('Ad impressions today.');
    $tokens['ad']['today_clicks']      = t('Ad clicks today.');
    $tokens['ad']['last_hour_impressions']   = t('Ad impressions last hour.');
    $tokens['ad']['last_hour_clicks']  = t('Ad clicks this hour.');
    $tokens['ad']['this_hour_impressions']   = t('Ad impressions this hour.');
    $tokens['ad']['this_hour_clicks']  = t('Ad clicks this hour.');
  }
  if ($type == 'ad' || $type == 'all') {
    $tokens['Ad owner']['owner_name']  = t('The username of the ad owner.');
    $tokens['Ad owner']['owner_mail']  = t('The email address of the ad owner.');
    $tokens['Ad owner']['owner_uid']   = t('The user ID of the ad owner.');
  }
  return $tokens;
}
