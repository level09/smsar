<?php
// $Id: ad.pages.inc,v 1.1.2.6.2.4 2009/11/30 16:31:20 Jeremy Exp $

/**
 * @file
 * Advertisement nodes pages and forms.
 *
 * Copyright (c) 2005-2009.
 *   Jeremy Andrews <jeremy@tag1consulting.com>
 */

function theme_node_ad($node, $yield_form = TRUE) {
  $output = '';
  if (ad_permission($node, 'access statistics')) {
    $output = theme('ad_status_display', $node);
    $output .= theme('ad_statistics_display', ad_statistics($node->nid));
  }
  if (ad_permission($node, 'access click history')) {
    $header = array(
      array('data' => t('Time'), 'field' => 'timestamp', 'sort' => 'desc'),
      array('data' => t('User'), 'field' => 'uid'),
      array('data' => t('URL where clicked'), 'field' => 'url'),
    );
    if (function_exists('click_filter_status_text') && user_access('view filtered clicks')) {
      $header[] = array('data' => t('Status'), 'field' => 'status');
    }
    $header[] = '';

    if (isset($node->nid) && $node->nid > 0) {
      $sql = "SELECT cid, timestamp, uid, status, hostname, url FROM {ad_clicks} WHERE aid = %d";
      $sql .= tablesort_sql($header);
      $result = pager_query($sql, 25, 0, NULL, $node->nid);

      while ($ad = db_fetch_object($result)) {
        if (module_exists('click_filter') && $ad->status != CLICK_VALID) {
          // Only show filtered clicks to users with permission to view them.
          if (!user_access('view filtered clicks')) {
            continue;
          }
        }
        if (strlen($ad->url) > 40) {
          $url = substr($ad->url, 0, 37) .'...';
        }
        else {
          $url = $ad->url;
        }
        $row = array();
        $click_user = user_load(array('uid' => $ad->uid));
        $row[] = format_date($ad->timestamp, 'custom', 'M j H:i');
        $row[] = theme('username', $click_user);
        $row[] = l($url, $ad->url);
        if (function_exists('click_filter_status_text') && user_access('view filtered clicks')) {
          $row[] = click_filter_status_text($ad->status);
        }
        $row[] = '['. l(t('details'), 'node/'. $node->nid .'/details/'. $ad->cid) .']';
        $rows[] = $row;
      }

      if (empty($rows)) {
        $click_history = '<p>'. t('There are no clicks yet.') .'</p>';
      }
      else {
        $click_history = theme('table', $header, $rows);
      }
      $click_history .= theme('pager', NULL, 25, 0);
      $output .= theme('box', t('Click history'), $click_history);
    }
  }
  return $output;
}

/**
 * Calculate statistics for the given advertisements.
 * TODO: Introduce caching to make this more efficient.
 */
function ad_statistics($aid) {
  // Get global statistics.
  $statistics['global']['views'] = (int)db_result(db_query("SELECT SUM(count) FROM {ad_statistics} WHERE aid = %d AND action = 'view'", $aid));
  $statistics['global']['clicks'] = (int)db_result(db_query("SELECT SUM(count) FROM {ad_statistics} WHERE aid = %d AND action = 'click'", $aid));

  // No sense in making further queries if the ad has no global statistics.
  if (!$statistics['global']['views'] && !$statistics['global']['clicks']) {
    return $statistics;
  }

  // Get statistics for this year and last year.
  $this_year = date('Y000000');
  $last_year = date('Y') - 1 .'000000';
  $statistics['last_year']['views'] = (int)db_result(db_query("SELECT SUM(count) FROM {ad_statistics} WHERE aid = %d AND action = 'view' AND date >= %d AND date <= %d", $aid, $last_year, $this_year));
  $statistics['last_year']['clicks'] = (int)db_result(db_query("SELECT SUM(count) FROM {ad_statistics} WHERE aid = %d AND action = 'click' AND date >= %d AND date <= %d", $aid, $last_year, $this_year));
  $statistics['this_year']['views'] = (int)db_result(db_query("SELECT SUM(count) FROM {ad_statistics} WHERE aid = %d AND action = 'view' AND date >= %d", $aid, $this_year));
  $statistics['this_year']['clicks'] = (int)db_result(db_query("SELECT SUM(count) FROM {ad_statistics} WHERE aid = %d AND action = 'click' AND date >= %d", $aid, $this_year));

  // No sense in making further queries if the ad has no statistics this year.
  if (!$statistics['this_year']['views'] && !$statistics['this_year']['clicks']) {
    return $statistics;
  }

  // Get statistics for this month and last month.
  $this_month = date('Ym0000');
  $last_month = date('m') - 1;
  if ($last_month == 0) {
    $last_month = date('Y') - 1 .'120000';
  }
  else {
    $last_month = date('Y') . ($last_month < 10 ? '0' : '') . $last_month .'0000';
  }
  $statistics['last_month']['views'] = (int)db_result(db_query("SELECT SUM(count) FROM {ad_statistics} WHERE aid = %d AND action = 'view' AND date >= %d AND date <= %d", $aid, $last_month, $this_month));
  $statistics['last_month']['clicks'] = (int)db_result(db_query("SELECT SUM(count) FROM {ad_statistics} WHERE aid = %d AND action = 'click' AND date >= %d AND date <= %d", $aid, $last_month, $this_month));
  $statistics['this_month']['views'] = (int)db_result(db_query("SELECT SUM(count) FROM {ad_statistics} WHERE aid = %d AND action = 'view' AND date >= %d", $aid, $this_month));
  $statistics['this_month']['clicks'] = (int)db_result(db_query("SELECT SUM(count) FROM {ad_statistics} WHERE aid = %d AND action = 'click' AND date >= %d", $aid, $this_month));

  // No sense in making further queries if the ad has no statistics this month.
  if (!$statistics['this_month']['views'] && !$statistics['this_month']['clicks']) {
    return $statistics;
  }

  // Get statistics for this week.
  $this_week_start = date('Ymd00', time() - 60*60*24*6);
  $statistics['this_week']['views'] = (int)db_result(db_query("SELECT SUM(count) FROM {ad_statistics} WHERE aid = %d AND action = 'view' AND date > %d", $aid, $this_week_start));
  $statistics['this_week']['clicks'] = (int)db_result(db_query("SELECT SUM(count) FROM {ad_statistics} WHERE aid = %d AND action = 'click' AND date > %d", $aid, $this_week_start));

  // No sense in making further queries if the ad has no statistics this week.
  if (!$statistics['this_week']['views'] && !$statistics['this_week']['clicks']) {
    return $statistics;
  }

  // Get statistics for yesterday and today.
  $yesterday_start = date('Ymd00', time() - 60*60*24);
  $yesterday_end = date('Ymd24', time() - 60*60*24);
  $today_start = date('Ymd00', time());
  $statistics['yesterday']['views'] = (int)db_result(db_query("SELECT SUM(count) FROM {ad_statistics} WHERE aid = %d AND action = 'view' AND date >= %d AND date <= %d", $aid, $yesterday_start, $yesterday_end));
  $statistics['yesterday']['clicks'] = (int)db_result(db_query("SELECT SUM(count) FROM {ad_statistics} WHERE aid = %d AND action = 'click' AND date >= %d AND date <= %d", $aid, $yesterday_start, $yesterday_end));
  $statistics['today']['views'] = (int)db_result(db_query("SELECT SUM(count) FROM {ad_statistics} WHERE aid = %d AND action = 'view' AND date >= %d", $aid, $today_start));
  $statistics['today']['clicks'] = (int)db_result(db_query("SELECT SUM(count) FROM {ad_statistics} WHERE aid = %d AND action = 'click' AND date >= %d", $aid, $today_start));

  // No sense in making further queries if the ad has no statistics today.
  if (!$statistics['today']['views'] && !$statistics['today']['clicks']) {
    return $statistics;
  }

  // Get statistics for this hour and the last hour.
  $last_hour = date('YmdH', time() - 60*60);
  $this_hour = date('YmdH', time());
  $statistics['last_hour']['views'] = (int)db_result(db_query("SELECT SUM(count) FROM {ad_statistics} WHERE aid = %d AND action = 'view' AND date = %d", $aid, $last_hour));
  $statistics['last_hour']['clicks'] = (int)db_result(db_query("SELECT SUM(count) FROM {ad_statistics} WHERE aid = %d AND action = 'click' AND date = %d", $aid, $last_hour));
  $statistics['this_hour']['views'] = (int)db_result(db_query("SELECT SUM(count) FROM {ad_statistics} WHERE aid = %d AND action = 'view' AND date = %d", $aid, $this_hour));
  $statistics['this_hour']['clicks'] = (int)db_result(db_query("SELECT SUM(count) FROM {ad_statistics} WHERE aid = %d AND action = 'click' AND date = %d", $aid, $this_hour));

  return $statistics;
}

function theme_ad_statistics_display($statistics) {
  $header = array('', t('Impressions'), t('Clicks'), t('Click-thru'));
  $rows = array();

  $data = array(
   'this_hour' => t('This hour'),
   'last_hour' => t('Last hour'),
   'today' => t('Today'),
   'yesterday' => t('Yesterday'),
   'this_week' => t('Last seven days'),
   'this_month' => t('This month'),
   'last_month' => t('Last month'),
   'this_year' => t('This year'),
   'last_year' => t('Last year'),
   'global' => t('All time')
  );

  foreach ($data as $key => $value) {
    if (isset($statistics[$key]) && (isset($statistics[$key]['views']) || isset($statistics[$key]['clicks']) || $key == 'global')) {
      $rows[] = array(
        array('data' => $value),
        array('data' => (int)$statistics[$key]['views']),
        array('data' => (int)$statistics[$key]['clicks']),
        array('data' => $statistics[$key]['views'] ? sprintf('%1.2f', ((int)$statistics[$key]['clicks'] / (int)$statistics[$key]['views']) * 100) .'%' : '0.00%'),
      );
    }
  }
  if (empty($rows) || (!$statistics['global']['views'] && !$statistics['global']['clicks'])) {
    $statistics = '<p>'. t('There are currently no statistics for this advertisement.') .'</p>';
  }
  else {
    $statistics = theme('table', $header, $rows);
  }

  return theme('box', t('Statistics'), $statistics);
}

/**
 * Display details about a specific click.
 */
function ad_click_details($node, $cid) {
  drupal_set_breadcrumb(array(l(t('Home'), NULL), l(check_plain($node->title), 'node/'. $node->nid)));
  if ($click = db_fetch_object(db_query('SELECT * FROM {ad_clicks} WHERE cid = %d', $cid))) {
    $ad = node_load($click->aid);
    $account = user_load(array('uid' => $click->uid));
    $rows = array(
      array(
        array('data' => t('Time'), 'header' => TRUE),
        format_date($click->timestamp, 'custom', 'D F j, Y h:i a'),
      ),
      array(
        array('data' => t('User'), 'header' => TRUE),
        theme('username', $account),
      ),
      array(
        array('data' => t('IP Address'), 'header' => TRUE),
        $click->hostname,
      ),
      array(
        array('data' => t('User Agent'), 'header' => TRUE),
        check_plain($click->user_agent),
      ),
      array(
        array('data' => t('URL'), 'header' => TRUE),
        l($click->url, $click->url),
      ),
      array(
        array('data' => t('Advertisement'), 'header' => TRUE),
        $ad->ad,
      )
    );
    if (function_exists('click_filter_status_text') && user_access('view filtered clicks')) {
      switch ($click->status) {
        case 0:
        default:
          $status = t('Not valid: this click has not been counted for unknown reasons.  This is an unexpected error.');
          break;
        case 1:
          $status = t('Valid: this is a valid click.');
          break;
        case 2:
          $status = t('Not valid: this click has not been counted because another click by the same IP address was already counted.');
          break;
        case 3:
          $status = t('Not valid: this click has not been counted because it was generated by an owner of the advertisement.');
          break;
        case 4:
          $status = t('Not valid: this click has not been counted because it was generated by a user in a filtered role.');
          break;
        case 5:
          $status = t('Not valid: this click has not been counted because it was generated by an automated "bot".');
          break;
      }
      $rows[] = array(array('data' => t('Status'), 'header' => TRUE), $status);
    }
    $output = theme('table', array(), $rows);
  }
  return $output;
}


function ad_activity_details($node) {
  $output = '';
  drupal_set_breadcrumb(array(l(t('Home'), NULL), l(check_plain($node->title), 'node/'. $node->nid)));
  if (ad_permission($node, 'access click history')) {
    $header = array(
      array('data' => t('Date'), 'field' => 'date', 'sort' => 'desc'),
      array('data' => t('Action'), 'field' => 'action'));
    if (isset($node->nid) && $node->nid > 0) {
      $sql = "SELECT * FROM {ad_statistics} WHERE action NOT IN ('view', 'click') AND aid = %d";
      $sql .= tablesort_sql($header);
      $result = pager_query($sql, 25, 0, NULL, $node->nid);
      while ($ad = db_fetch_object($result)) {
        $row = array();
        $row[] = format_date(strtotime($ad->date . '00'), 'large');
        $row[] = $ad->action;
        $rows[] = $row;
      }
      if (empty($rows)) {
        $output = '<p>'. t('There is no activity yet.') .'</p>';
      }
      else {
        $output = theme('table', $header, $rows);
      }
      $output .= theme('pager', NULL, 25, 0);
    }
  }
  return theme('box', t('Activity'), $output);
}

