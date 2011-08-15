<?php
// $Id: taxonomy_facets.admin.inc,v 1.1 2008/12/29 18:09:38 davidlesieur Exp $

/**
 * @file
 * Administrative page callbacks for the Taxonomy Facets module.
 */

// TODO: Provide indexing status page a la search.module

/**
 * Menu callback for the index page.
 */
function taxonomy_facets_index_page() {
  if (count(taxonomy_get_vocabularies())) {
    $form['indexing'] = array(
      '#type' => 'fieldset',
      '#title' => t('Indexing'),
    );
    $form['indexing']['taxonomy_facets_rebuild'] = array(
      '#type' => 'markup',
      '#value' => t('If you are getting inconsistent results when navigating taxonomy facets, try to <a href="@rebuild">rebuild the Taxonomy Facets index</a>. However, inconsistencies might denote a bug; please report any bug to Faceted Search\'s <a href="http://drupal.org/project/issues/faceted_search">issue queue</a>.', array('@rebuild' => url('admin/settings/faceted_search/taxonomy_facets/rebuild'))),
    );
    // TODO: Redo as POST request.
    return drupal_render($form);
  }
  else {
    return '<p>'. t('There are currently no vocabularies. Go to the !page first to create vocabularies.', array('!page' => l(t('Categories page'), 'admin/content/taxonomy'))) .'</p>';
  }
}

/**
 * Menu callback for marking all term node associations as needing an update.
 */
function taxonomy_facets_rebuild($goto_admin = TRUE) {
  // Mark nodes with the current time, unless they're already marked.
  db_query('UPDATE {taxonomy_facets_node} SET changed = %d WHERE changed = 0', time());
  drupal_set_message(t('Indexing will start on the next <a href="@cron">cron</a> run.', array('@cron' => url('admin/reports/status/run-cron', array('query' => drupal_get_destination())))));
  if ($goto_admin) {
    drupal_goto('admin/settings/faceted_search/taxonomy_facets');
  }
}

