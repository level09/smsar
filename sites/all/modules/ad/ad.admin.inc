<?php
// $Id: ad.admin.inc,v 1.1.2.9.2.7 2009/11/23 06:40:02 Jeremy Exp $

/**
 * @file
 * Advertisement admin pages and functions.
 *
 * Copyright (c) 2005-2009.
 *   Jeremy Andrews <jeremy@tag1consulting.com>.
 */

/**
 * Build default ad administration page.
 */
function ad_admin_list() {
  _ad_check_installation();

  $output = drupal_get_form('ad_filter_form');

  if (isset($_POST['operation']) && ($_POST['operation'] == 'delete') && isset($_POST['ads'])) {
    return drupal_get_form('ad_multiple_delete_confirm');
  }
  $output .= drupal_get_form('ad_admin_ads');

  return $output;
}

/**
 * Provide a filterable list of advertisements.
 */
function ad_admin_ads() {
  $filter = ad_build_filter_query();
  $result = pager_query('SELECT a.*, n.* FROM {ads} a INNER JOIN {node} n ON a.aid = n.nid '. $filter['join'] .' '. $filter['where'] .' ORDER BY n.changed DESC', 50, 0, NULL, $filter['args']);

  $form['options'] = array('#type' => 'fieldset',
    '#title' => t('Update options'),
    '#prefix' => '<div class="container-inline">',
    '#suffix' => '</div>',
  );
  $options = array();
  foreach (module_invoke_all('ad_operations') as $operation => $array) {
    $options[$operation] = $array['label'];
  }
  $form['options']['operation'] = array('#type' => 'select', '#options' => $options,  '#default_value' => 'approve');
  $form['options']['submit'] = array('#type' => 'submit', '#value' => t('Update'));

  $destination = drupal_get_destination();
  $ads = array();
  while ($ad = db_fetch_object($result)) {
    $ads[$ad->aid] = '';
    $form['title'][$ad->aid] = array('#value' => l($ad->title, 'node/'. $ad->aid));
    $form['group'][$ad->aid] = array('#value' => _ad_get_group($ad->aid));
    $form['adtype'][$ad->aid] = array('#value' => t(check_plain($ad->adtype)));
    $form['adstatus'][$ad->aid] = array('#value' => t(check_plain($ad->adstatus)));
    $form['operations'][$ad->aid] = array('#value' => l(t('edit'), 'node/'. $ad->aid .'/edit', array('query' => $destination)));
  }
  $form['ads'] = array('#type' => 'checkboxes', '#options' => $ads);
  $form['pager'] = array('#value' => theme('pager', NULL, 50, 0));
  return $form;
}

/**
 * Implementation of hook_ad_operations().
 */
function ad_ad_operations() {
  $operations = array(
    'approved' => array(
      'label' => t('Mark as approved'),
      'callback' => 'ad_operations_callback',
      'callback arguments' => array('approved'),
    ),
    'active' => array(
      'label' => t('Mark as active'),
      'callback' => 'ad_operations_callback',
      'callback arguments' => array('active'),
    ),
    'expired' => array(
      'label' => t('Mark as expired'),
      'callback' => 'ad_operations_callback',
      'callback arguments' => array('expired'),
    ),
    'pending' => array(
      'label' => t('Mark as pending'),
      'callback' => 'ad_operations_callback',
      'callback arguments' => array('pending'),
    ),
    'offline' => array(
      'label' => t('Mark as offline'),
      'callback' => 'ad_operations_callback',
      'callback arguments' => array('offline'),
    ),
    'denied' => array(
      'label' => t('Mark as denied'),
      'callback' => 'ad_operations_callback',
      'callback arguments' => array('denied'),
    ),
    'delete' => array(
      'label' => t('Delete'),
    ),
  );
  return $operations;
}

/**
 * Callback function for admin mass approving ads.
 * TODO: Update activated and expired when appropriate.
 * TODO: Publish/unpublish nodes when appropriate.
 */
function ad_operations_callback($ads, $action) {
  $placeholders = implode(',', array_fill(0, count($ads), '%d'));
  db_query("UPDATE {ads} SET adstatus = '". $action ."' WHERE aid IN(". $placeholders .')', $ads);
  foreach ($ads as $aid) {
    $node = node_load($aid);
    ad_statistics_increment($aid, 'update');
    ad_statistics_increment($aid, $action);
    // Allow ad type module to act on nodeapi events.  The adapi hook provides
    // access to additional variables not available in the nodeapi hook.
    if (isset($node->adtype)) {
      // Don't use module_invoke, as in pre-PHP5 the changes to $node won't be
      // passed back.
      $function = "ad_$node->adtype" .'_adapi';
      if (function_exists($function)) {
        $function('update', $node);
      }
    }
    // Allow ad cache module to act on nodeapi events.
    $cache = variable_get('ad_cache', 'none');
    if ($cache != 'none') {
      $function = "ad_cache_$cache" .'_adcacheapi';
      if (function_exists($function)) {
        $function($action, $node);
      }
    }
  }
}

/**
 * Display a form to confirm whether to really delete the selected ads.
 */
function ad_multiple_delete_confirm($form_state) {
  $form['ads'] = array('#prefix' => '<ul>', '#suffix' => '</ul>', '#tree' => TRUE);
  // array_filter returns only elements with TRUE values
  foreach (array_filter($form_state['post']['ads']) as $aid => $value) {
    $title = db_result(db_query('SELECT title FROM {node} WHERE nid = %d', $aid));
    $form['ads'][$aid] = array('#type' => 'hidden', '#value' => $aid, '#prefix' => '<li>', '#suffix' => check_plain($title) ."</li>\n");
  }
  $form['operation'] = array('#type' => 'hidden', '#value' => 'delete');

  return confirm_form($form,
                      t('Are you sure you want to delete these ads?'),
                      'admin/content/ad', t('This action cannot be undone.'),
                      t('Delete all'), t('Cancel'));
}

/**
 * Perform the actual ad deletions.
 */
function ad_multiple_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    foreach ($form_state['values']['ads'] as $aid => $value) {
      node_delete($aid);
    }
    drupal_set_message(t('The ads have been deleted.'));
  }
  $form_state['redirect'] = 'admin/content/ad';
}

/**
 * Theme ad administration overview.
 */
function theme_ad_admin_ads($form) {
  // Overview table:
  $header = array(theme('table_select_header_cell'), t('Title'), t('Group'), t('Type'), t('Status'), t('Operations'));

  $output = drupal_render($form['options']);
  if (isset($form['title']) && is_array($form['title'])) {
    foreach (element_children($form['title']) as $key) {
      $row = array();
      $row[] = drupal_render($form['ads'][$key]);
      $row[] = drupal_render($form['title'][$key]);
      $row[] = drupal_render($form['group'][$key]);
      $row[] = drupal_render($form['adtype'][$key]);
      $row[] = drupal_render($form['adstatus'][$key]);
      $row[] = drupal_render($form['operations'][$key]);
      $rows[] = $row;
    }

  }
  else  {
    $rows[] = array(array('data' => t('No ads available.'), 'colspan' => '6'));
  }

  $output .= theme('table', $header, $rows);
  if ($form['pager']['#value']) {
    $output .= drupal_render($form['pager']);
  }

  $output .= drupal_render($form);

  return $output;
}

/**
 * Must select an ad if performing an operation.
 */
function ad_admin_ads_validate($form, &$form_state) {
  $ads = array_filter($form_state['values']['ads']);
  if (count($ads) == 0) {
    form_set_error('', t('No ads selected.'));
  }
}

/**
 * Submit the ad administration update form.
 */
function ad_admin_ads_submit($form, &$form_state) {
  $operations = module_invoke_all('ad_operations');
  $operation = $operations[$form_state['values']['operation']];
  // Filter out unchecked nodes
  $ads = array_filter($form_state['values']['ads']);
  if ($function = $operation['callback']) {
    // Add in callback arguments if present.
    if (isset($operation['callback arguments'])) {
      $args = array_merge(array($ads), $operation['callback arguments']);
    }
    else {
      $args = array($ads);
    }
    call_user_func_array($function, $args);

    cache_clear_all();
    ad_rebuild_cache(TRUE);
    drupal_set_message(t('The update has been performed.'));
  }
}

/**
 * Build query for ad administration filters based on session.
 */
function ad_build_filter_query() {
  $filters = ad_filters();

  // Build query
  $where = $args = array();
  $join = '';
  foreach ($_SESSION['ad_overview_filter'] as $index => $filter) {
    list($key, $value) = $filter;
    switch ($key) {
      case 'status':
        list($value, $key) = explode('-', $value, 2);
        $op = $key == 1 ? '=' : '!=';
        $where[] = "a.adstatus $op '%s'";
        break;
      case 'group':
        $table = "tn$index";
        $where[] = "$table.tid = %d";
        $join .= "INNER JOIN {term_node} $table ON n.nid = $table.nid ";
        break;
      case 'type':
        $where[] = "a.adtype = '%s'";
      default:
        $return = module_invoke_all('adapi', 'admin_filter_query', $filter);
        foreach ($return as $module => $funcs) {
          if (isset($funcs['where'])) {
            $where[] = $funcs['where'];
          }
          if (isset($funcs['join'])) {
            $join .= $funcs['join'];
          }
          if (isset($funcs['value'])) {
            $value = $funcs['value'];
          }
        }
        break;
    }
    $args[] = $value;
  }
  $where = count($where) ? 'WHERE '. implode(' AND ', $where) : '';

  return array('where' => $where, 'join' => $join, 'args' => $args);
}

/**
 * List ad administration filters that can be applied.
 */
function ad_filters() {
  $session = &$_SESSION['ad_overview_filter'];
  $session = is_array($session) ? $session : array();
  // Regular filters
  $options = array(
    'pending-1' => t('pending'),
    'approved-1' => t('approved'),
    'active-1' => t('active'),
    'offline-1' => t('offline'),
    'expired-1' => t('expired'),
    'denied-1' => t('denied'),
    'pending-0' => t('not pending'),
    'approved-0' => t('not approved'),
    'active-0' => t('not active'),
    'offline-0' => t('not offline'),
    'expired-0' => t('not expired'),
    'denied-0' => t('not denied')
  );

  $filters['status'] = array(
    'title' => t('status'),
    'options' => $options
  );
  $adtypes = ad_get_types();
  $filters['type'] = array(
    'title' => t('type'),
    'options' => $adtypes,
  );
  // The taxonomy filter
  if ($taxonomy = module_invoke('taxonomy', 'get_tree', _ad_get_vid())) {
    $options = array();
    // TODO: Add support for the default group.
    //$options[0] = t('default');
    foreach ($taxonomy as $term) {
      $options[$term->tid] = check_plain($term->name);
    }
    $filters['group'] = array('title' => t('group'), 'options' => $options);
  }

  $filters = array_merge($filters, module_invoke_all('adapi', 'admin_filters', array()));
  return $filters;
}

/**
 * Theme ad administration filter selector.
 */
function theme_ad_filters($form) {
  $output = '<ul class="clear-block">';
  if (isset($form['current']) && sizeof($form['current'])) {
    foreach (element_children($form['current']) as $key) {
      $output .= '<li>'. drupal_render($form['current'][$key]) .'</li>';
    }
  }

  $output .= '<li><dl class="multiselect">'. (isset($form['current']) && sizeof($form['current']) ? '<dt><em>'. t('and') .'</em> '. t('where') .'</dt>' : '') .'<dd class="a">';
  foreach (element_children($form['filter']) as $key) {
    $output .= drupal_render($form['filter'][$key]);
  }
  $output .= '</dd>';

  $output .= '<dt>'. t('is') .'</dt><dd class="b">';

  if (isset($form['status'])) {
    foreach (element_children($form['status']) as $key) {
      $output .= drupal_render($form['status'][$key]);
    }
  }
  $output .= '</dd>';

  $output .= '</dl>';
  $output .= '<div class="container-inline" id="ad-admin-buttons">'. drupal_render($form['buttons']) .'</div>';
  $output .= '</li></ul>';

  return $output;
}

/**
 * Return form for advertisement administration filters.
 */
function ad_filter_form($form_state) {
  $session = &$_SESSION['ad_overview_filter'];
  $session = is_array($session) ? $session : array();
  $filters = ad_filters();

  $i = 0;
  $form['filters'] = array('#type' => 'fieldset',
    '#title' => t('Show only ads where'),
    '#theme' => 'ad_filters',
  );
  foreach ($session as $filter) {
    list($type, $value) = $filter;
    if ($type == 'category') {
      // Load term name from DB rather than search and parse options array.
      $value = module_invoke('taxonomy', 'get_term', $value);
      $value = $value->name;
    }
    else if ($type == 'status') {
      $value = $filters['status']['options'][$value];
    }
    else {
      $value = $filters[$type]['options'][$value];
    }
    $string = ($i++ ? '<em>and</em> where <strong>%a</strong> is <strong>%b</strong>' : '<strong>%a</strong> is <strong>%b</strong>');
    $form['filters']['current'][] = array('#value' => t($string, array('%a' => $filters[$type]['title'] , '%b' => $value)));
    if ($type == 'type') {
      // Remove the type option if it is already being filtered on.
      unset($filters['type']);
    }
    else if ($type == 'group') {
      unset($filters['group']);
    }
    if ($type == 'status') {
      foreach ($session as $option) {
        if ($option[0] == 'status') {
          list($value, $key) = explode('-', $option[1], 2);
          if ($key) {
            // One postive key means we can't have any more.
            // Remove the status option if we're already filtering on a positive
            // key (ie, 'active', as an ad can't be 'active' and 'pending')
            unset($filters['status']);
          }
          else {
            // When a key is selected, remove it and its inverse as there's
            // no logic in selecting the same key multiple times, and selecting
            // two opposite keys will always return 0 results.
            $inverse = $key == 1 ? 0 : 1;
            unset($filters['status']['options'][$option[1]]);
            unset($filters['status']['options'][$value .'-'. $inverse]);
          }
        }
      }
    }
  }

  $names = array();
  foreach ($filters as $key => $filter) {
    $names[$key] = $filter['title'];
    $form['filters']['status'][$key] = array('#type' => 'select', '#options' => $filter['options']);
  }

  $form['filters']['filter'] = array('#type' => 'radios', '#options' => $names, '#default_value' => 'status');
  $form['filters']['buttons']['submit'] = array('#type' => 'submit', '#value' => (count($session) ? t('Refine') : t('Filter')));
  if (count($session)) {
    $form['filters']['buttons']['undo'] = array('#type' => 'submit', '#value' => t('Undo'));
    $form['filters']['buttons']['reset'] = array('#type' => 'submit', '#value' => t('Reset'));
  }

  return $form;
}

/**
 * Theme ad administration filter form.
 */
function theme_ad_filter_form($form) {
  $output  = '<div id="ad-admin-filter">';
  $output .= drupal_render($form['filters']);
  $output .= '</div>';
  $output .= drupal_render($form);
  return $output;
}

/**
 * Process result from ad administration filter form.
 */
function ad_filter_form_submit($form, &$form_state) {
  $filters = ad_filters();
/* TODO The 'op' element in the form values is deprecated.
   Each button can have #validate and #submit functions associated with it.
   Thus, there should be one button that submits the form and which invokes
   the normal form_id_validate and form_id_submit handlers. Any additional
   buttons which need to invoke different validate or submit functionality
   should have button-specific functions. */
  switch ($form_state['values']['op']) {
    case t('Filter'):
    case t('Refine'):
      if (isset($form_state['values']['filter'])) {
        $filter = $form_state['values']['filter'];

        // Flatten the options array to accommodate hierarchical/nested options.
        $flat_options = form_options_flatten($filters[$filter]['options']);

        if (isset($form_state['values'][$filter]) && isset($flat_options[$form_state['values'][$filter]])) {
          $_SESSION['ad_overview_filter'][] = array($filter, $form_state['values'][$filter]);
        }
      }
      break;
    case t('Undo'):
      array_pop($_SESSION['ad_overview_filter']);
      break;
    case t('Reset'):
      $_SESSION['ad_overview_filter'] = array();
      break;
  }
}

/**
 * Display a form for the ad module settings.
 */
function ad_admin_configure_settings($form_state) {
  _ad_check_installation();

  $adserve = variable_get('adserve', '');
  $adserveinc = variable_get('adserveinc', '');
  $form['configuration'] = array(
    '#type' => 'fieldset',
    '#title' => t('Status'),
  );
  $form['configuration']['adserve'] = array(
    '#type' => 'markup',
    '#value' => t('Using detected adserve scripts: %adserve, %adserveinc', array('%adserve' => ($adserve ? $adserve : t('not found')), '%adserveinc' => ($adserveinc ? $adserveinc : t('not found')))),
  );

  $form['general'] = array(
    '#type' => 'fieldset',
    '#title' => t('General'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  // TODO: This needs a per-group over-ride, in case some groups are IFrames,
  // while others are JavaScript, etc.
  $form['general']['ad_link_target'] = array(
    '#type' => 'radios',
    '#title' => t('Click-through target'),
    '#options' => array(
      '_self' => t('same browser window and frame'),
      '_blank' => t('new browser window'),
      '_parent' => t('parent frame'),
      '_top' => t('same browser window, removing all frames'),
    ),
    '#default_value' => variable_get('ad_link_target', '_self'),
    '#description' => t('Select an option above to configure what happens when an ad is clicked.  These options set the <em>a target</em>, and are <em>_self</em>, <em>_blank</em>, <em>_parent</em> and <em>_top</em> respectively.'),
  );

  $form['general']['ad_link_nofollow'] = array(
    '#type' => 'checkbox',
    '#title' => t('nofollow'),
    '#default_value' => variable_get('ad_link_nofollow', 0),
    '#description' => t('If enabled, %tag will be added to advertisement links generated by this module.', array('%tag' => t('rel="nofollow"'))),
  );

  // Provide hook for ad_display_TYPE modules to set display TYPE.
  $display_options = array_merge(array('javascript' => t('JavaScript'), 'jquery' => t('jQuery'), 'iframe' => t('IFrame'), 'raw' => t('Raw')), module_invoke_all('displayapi', 'display_method'), array());

  // Provide hook for ad_display_TYPE modules to define inline description.
  $description = t('This setting configures the default method for displaying advertisements on your website.  It is possible to override this setting when making direct calls to ad(), as described in the documentation.  Using the JavaScript, jQuery, and IFrame display methods allows you to display random ads and track impressions even on cached pages.  When using the Raw display method together with Drupal\'s page cache, impressions will be properly tracked but advertisements will only change when the page cache is updated.');
  $return = module_invoke_all('displayapi', 'display_description', array());
  foreach ($return as $describe) {
    $description .= ' '. $describe;
  }

  $form['general']['ad_display'] = array(
    '#type' => 'radios',
    '#title' => t('Display type'),
    '#default_value' => variable_get('ad_display', 'javascript'),
    '#options' => $display_options,
    '#description' => $description,
  );

  $form['general']['ad_validate_url'] = array(
    '#type' => 'checkbox',
    '#title' => t('Validate URLs'),
    '#default_value' => variable_get('ad_validate_url', 1),
    '#description' => t('If enabled, any destination URLs entered in ads will be required to be complete URLs (including http:// or https:// at the beginning).  If you wish to include internal urls, you will need to disable this option.'),
  );

  $form['search'] = array(
    '#type' => 'fieldset',
    '#title' => t('Search'),
    '#collapsible' => TRUE,
  );

  $form['search']['ad_no_search'] = array(
    '#type' => 'checkbox',
    '#title' => t('Remove ads from local search results'),
    '#default_value' => variable_get('ad_no_search', 1),
    '#description' => t('If enabled, all advertisements will be removed from local search results.  Users with %perm1 or %perm2 permissions will still see advertisements in the search results.', array('%perm1' => t('administer advertisements'), '%perm2' => t('edit any advertisement'))),
  );

  $form['search']['ad_meta_noindex'] = array(
    '#type' => 'checkbox',
    '#title' => t('Remove ads from remote search engines'),
    '#default_value' => variable_get('ad_meta_noindex', 0),
    '#description' => t('If enabled, advertisement nodes will include the !noindex, telling remote search engines to not include these nodes in their search index.  This meta tag will appear on any page that an advertisement node is displayed on, even if a page is comprised of both ad nodes and other site content.  It will not be displayed when advertisements are displayed via blocks, calls to ad(), or other display methods when the page is comprised of non-ad content. This option is disabled by default as users with this option enabled and custom home pages have accidentally caused important pages such as their front page to not be indexed by popular search engines.', array('!noindex' => l(t('noindex meta tag'), 'http://en.wikipedia.org/wiki/Noindex'))),
  );

  $form['iframe'] = array(
    '#type' => 'fieldset',
    '#title' => t('IFrame'),
    '#collapsible' => TRUE,
    '#collapsed' => variable_get('ad_display', 'javascript') == 'iframe' ? FALSE : TRUE
  );
  $form['iframe']['ad_iframe_frameborder'] = array(
    '#type' => 'checkbox',
    '#title' => t('Frameborder'),
    '#default_value' => variable_get('ad_iframe_frameborder', 0),
    '#description' => t('If enabled, IFrames used for displaying ads will have a frameborder.'),
  );
  $form['iframe']['ad_iframe_scroll'] = array(
    '#type' => 'radios',
    '#title' => t('Scrolling'),
    '#default_value' => variable_get('ad_iframe_scroll', 'auto'),
    '#options' => array('auto' => 'auto', 'on' => 'on', 'off' => 'off'),
    '#description' => t('Define whether or not scroll bars should be enabled for the ad IFrame.'),
  );
  $form['iframe']['ad_iframe_width'] = array(
    '#type' => 'textfield',
    '#title' => t('Width'),
    '#default_value' => variable_get('ad_iframe_width', ''),
    '#maxlength' => 8,
    '#size' => 5,
    '#required' => FALSE,
    '#description' => t('The default width for advertisement IFrames'),
  );
  $form['iframe']['ad_iframe_height'] = array(
    '#type' => 'textfield',
    '#title' => t('Height'),
    '#default_value' => variable_get('ad_iframe_height', ''),
    '#maxlength' => 8,
    '#size' => 5,
    '#required' => FALSE,
    '#description' => t('The default height for advertisement IFrames'),
  );

  $form['cache'] = array(
    '#type' => 'fieldset',
    '#title' => t('Cache'),
    '#collapsible' => TRUE,
    '#collapsed' => variable_get('ad_cache', 'none') == 'none' ? TRUE : FALSE,
  );

  // Provide hook for ad_cache_TYPE modules to set cache TYPE.
  $cache_options = array_merge(array('none' => t('None')), module_invoke_all('adcacheapi', 'method', array()));

  // Provide hook for ad_cache_TYPE modules to define inline description.
  $description = t('A cache can be used to efficiently track how many times advertisements are displayed and clicked.');
  $return = module_invoke_all('adcacheapi', 'description', array());
  foreach ($return as $describe) {
    $description .= ' '. $describe;
  }

  $form['cache']['ad_cache'] = array(
    '#type' => 'radios',
    '#title' => t('Type'),
    '#default_value' => variable_get('ad_cache', 'none'),
    '#options' => $cache_options,
    '#description' => $description,
  );

  // Provide hook for ad_cache_TYPE modules to add inline settings.
  $form['cache'] = array_merge($form['cache'], module_invoke_all('adcacheapi', 'settings'));

  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Validate form settings, calling attention to any illogical configurations.
 */
function ad_admin_configure_settings_validate($form, &$form_state) {
  if ($form_state['values']['ad_link_target'] == '_self' &&
      $form_state['values']['ad_display'] == 'iframe') {
    // We don't consider this an error, as this could be exactly what the
    // administrator is trying to do.  But as for most people it is likely
    // to be a misconfiguration, display a helpful warning...
    drupal_set_message(t('You have configured your advertisements to be displayed in iframes, and you have configured your click-through target as "same browser window and frame".  This is an unusual configuration, as when you click your advertisements only the IFrame will be redirected.  Be sure that this is actually what you are trying to do.'));
  }
}

/**
 * Save updated values from settings form.
 */
function ad_admin_configure_settings_submit($form, &$form_state) {
  variable_set('ad_link_target', $form_state['values']['ad_link_target']);
  variable_set('ad_link_nofollow', $form_state['values']['ad_link_nofollow']);
  variable_set('ad_cache', $form_state['values']['ad_cache']);
  variable_set('ad_display', $form_state['values']['ad_display']);
  variable_set('ad_validate_url', $form_state['values']['ad_validate_url']);
  variable_set('ad_no_search', $form_state['values']['ad_no_search']);
  variable_set('ad_meta_noindex', $form_state['values']['ad_meta_noindex']);
  variable_set('ad_iframe_frameborder', $form_state['values']['ad_iframe_frameborder']);
  variable_set('ad_iframe_scroll', $form_state['values']['ad_iframe_scroll']);
  variable_set('ad_iframe_width', $form_state['values']['ad_iframe_width']);
  variable_set('ad_iframe_height', $form_state['values']['ad_iframe_height']);
  if (($cache = variable_get('ad_cache', 'none')) != 'none') {
    // Allow external cache types to store their settings
    module_invoke('ad_cache_'. $cache, 'adcacheapi', 'settings_submit', $form_state['values']);
  }
/*
 // TODO: Write an external display module and implement this.
  $display = variable_get('ad_display', 'javascript');
  if ($display != 'javascript' && $display != 'raw') {
    // Allow external display types to store their settings
    module_invoke('ad_cache_'. $cache, 'adcacheapi', 'settings_submit', $form_state['values']);
  }*/
}

/**
 * Empty page for ad_type modules that don't define a global settings page.
 * This way admins can still set default permissions for this ad type.
 */
function ad_no_global_settings($form_state) {
  $form = array();

  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

function ad_admin_groups_list() {
  _ad_check_installation();

  $header = array(
      array('data' => t('Name'), 'field' => 'name'),
      array('data' => t('Description'), 'field' => 'description'),
      array('data' => t('Options')),
    );

  $groups = taxonomy_get_tree(_ad_get_vid());

  if ($groups != array()) {
    foreach ($groups as $group) {
      $row = array();
      $row[] = check_plain($group->name);
      $row[] = check_plain($group->description);
      $row[] = l(t('edit'), "admin/content/ad/groups/$group->tid/edit");
      $rows[] = $row;
    }
  }
  else {
    $rows[] = array(array('data' => t('No groups have been created.'), 'colspan' => 3));
  }

  $output = theme('table', $header, $rows);
  $output .= theme('pager', NULL, 15, 0);

  return $output;
}

/**
 * Returns a form for adding an ad group.
 */
function ad_admin_group_form($form_state, $group = NULL) {
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Group name'),
    '#default_value' => isset($group->name) ? check_plain($group->name) : '',
    '#maxlength' => 64,
    '#required' => TRUE,
    '#description' => t('Specify a name for the ad group.')
  );

  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#default_value' => isset($group->description) ? check_plain($group->description) : '',
    '#required' => TRUE,
    '#description' => t('Describe this ad group.')
  );

  $form['weight'] = array(
    '#type' => 'weight',
    '#title' => t('Weight'),
    '#default_value' => isset($group->weight) ? $group->weight : 0,
    '#description' => t('When listing ad groups, those with lighter (smaller) weights get listed before ad groups with heavier (larger) weights.  Ad groups with equal weights are sorted alphabetically.')
  );

  $form['vid'] = array(
    '#type' => 'hidden',
    '#value' => _ad_get_vid(),
  );


  if (isset($group->tid)) {
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
    );
    $form['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
    );
    $form['tid'] = array(
      '#type' => 'value',
      '#value' => $group->tid
    );
  }
  else {
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Create group'),
    );
  }

  return $form;
}

/**
 * Save a newly created ad group.
 */
function ad_admin_group_form_validate($form, &$form_state) {
  if ($form_state['values']['op'] == t('Delete')) {
    drupal_goto('admin/content/ad/groups/'. $form_state['values']['tid'] .'/delete');
  }
}


/**
 * Save a newly created ad group.
 */
function ad_admin_group_form_submit($form, &$form_state) {
  $status = taxonomy_save_term($form_state['values']);
  switch ($status) {
    case SAVED_NEW:
      $groups = variable_get('ad_groups', array());
      $groups[] = $form_state['values']['tid'];
      variable_set('ad_groups', $groups);
      drupal_set_message(t('Created new ad group %term.', array('%term' => $form_state['values']['name'])));
      break;
    case SAVED_UPDATED:
      drupal_set_message(t('The ad group %term has been updated.', array('%term' => $form_state['values']['name'])));
  }
  $form_state['redirect'] = 'admin/content/ad/groups';
}

/**
 * Returns a confirmation page when deleting an ad group and all of its ads.
 */
function ad_confirm_group_delete($form_state, $group = NULL) {
  $form['tid'] = array(
    '#type' => 'value',
    '#value' => $group->tid,
  );
  $form['name'] = array(
    '#type' => 'value',
    '#value' => check_plain($group->name),
  );

  return confirm_form(
    $form,
    t('Are you sure you want to delete the ad group %name?', array('%name' => $group->name)),
    'admin/content/ad/groups',
    t('Ads that were within this group will not be deleted.  This action cannot be undone.'),
    t('Delete'),
    t('Cancel'));
}

/**
 * Delete ad group.
 */
function ad_confirm_group_delete_submit($form, &$form_state) {
  taxonomy_del_term($form_state['values']['tid']);
  drupal_set_message(t('The ad group %term has been deleted.', array('%term' => $form_state['values']['name'])));
  watchdog('ad', 'mailarchive: deleted %term ad group.', array('%term' => $form_state['values']['name']));

  $form_state['redirect'] = 'admin/content/ad/groups';
}

