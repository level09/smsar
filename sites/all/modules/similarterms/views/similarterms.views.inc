<?php
// $Id: similarterms.views.inc,v 1.1.2.1 2010/09/09 13:58:19 jjeff Exp $

/**
 * @file Similar By Terms Views data include file
 */

function similarterms_views_data() {
  
  // Basic table information.
  $data['similarterms']['table']['group']  = t('Similar By Terms');

  // what's this do?
  $data['similarterms']['table']['join'] = array(
    '#global' => array(),
  );
  
  $data['similarterms']['similarterms'] = array(
    'title' => t('Similarity'),
    'help' => t('Percentage/count of terms which node has in common with node given as argument.'),
    'field' => array(
      'handler' => 'similarterms_handler_field_similar',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'similarterms_handler_sort_similar',
    ),
  );
  
  // nid
  $data['similarterms']['nid'] = array(
    'title' => t('Nid'),
    'help' => t('ID of content item(s). Passes term ids to Similar By Terms.'), // The help that appears on the UI,
    
    // Information for accepting a nid as an argument
    'argument' => array(
      'handler' => 'similarterms_handler_argument_node_nid',
      'parent' => 'views_handler_argument_numeric', // make sure parent is included
      'name field' => 'title', // the field to display in the summary.
      'numeric' => TRUE,
      'validate type' => 'nid',
    ),

  );

  return $data;
}

/**
 * Implementation of hook_views_handlers().
 */
function similarterms_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'similarterms') . '/views',
    ),
    'handlers' => array(
      'similarterms_handler_sort_similar' => array(
        'parent' => 'views_handler_sort',
      ),
      'similarterms_handler_argument_node_nid' => array(
        'parent' => 'views_handler_argument_numeric',
      ),
      'similarterms_handler_field_similar' => array(
        'parent' => 'views_handler_field',
      ),
    ),
  );
}
