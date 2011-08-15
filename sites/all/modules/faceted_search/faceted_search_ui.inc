<?php
// $Id: faceted_search_ui.inc,v 1.4 2008/09/28 01:04:28 davidlesieur Exp $

/**
 * @file
 * Provides classes needed by the Faceted Search UI module, and that are
 * sometimes extended by other modules.
 */

/**
 * Base class for display styles.
 */
class faceted_search_ui_style {
  /**
   * Return the number of nodes per page.
   */
  function get_limit() {
    return variable_get('default_nodes_main', 10);
  }
}

/**
 * Provides the 'extracts' display style for search results.
 */
class faceted_search_ui_extract_style extends faceted_search_ui_style {

  /**
   * Return the name of this style.
   */
  function get_label() {
    return t('Extracts');
  }

  /**
   * Format the search results to display node extracts showing relevant
   * keywords.
   */
  function format_results($search) {
    drupal_add_css(drupal_get_path('module', 'search') .'/search.css');

    $found_items = $search->load_results($this->get_limit());

    // Taken from node_search() (node.module 1.764) - BEGIN
    $results = array();
    foreach ($found_items as $item) {
      // Build the node body.
      $node = node_load($item->nid);
      $node->build_mode = NODE_BUILD_SEARCH_RESULT;
      $node = node_build_content($node, FALSE, FALSE);
      $node->body = drupal_render($node->content);

      // Fetch comments for snippet
      $node->body .= module_invoke('comment', 'nodeapi', $node, 'update index');
      // Fetch terms for snippet
      $node->body .= module_invoke('taxonomy', 'nodeapi', $node, 'update index');

      $extra = node_invoke_nodeapi($node, 'search result');
      $results[] = array(
        'link' => url('node/'. $item->nid, array('absolute' => TRUE)),
        'type' => check_plain(node_get_types('name', $node)),
        'title' => $node->title,
        'user' => theme('username', $node),
        'date' => $node->changed,
        'node' => $node,
        'extra' => $extra,
        'score' => $item->score,
        'snippet' => search_excerpt(implode(' ', $search->get_keywords()), $node->body),
      );
    }
    // Taken from node_search() - END

    return theme('faceted_search_ui_search_page', $results, 'node');
  }
}

/**
 * Provides the 'teasers' display style for search results.
 */
class faceted_search_ui_teaser_style extends faceted_search_ui_style {

  /**
   * Return the name of this style.
   */
  function get_label() {
    return t('Teasers');
  }

  /**
   * Format the search results to display node teasers.
   */
  function format_results($search) {
    $limit = $this->get_limit();
    $found_items = $search->load_results($limit);

    foreach ($found_items as $item) {
      $output .= node_view(node_load($item->nid), TRUE);
    }
    return $output . theme('pager', NULL, $limit);
  }
}
