<?php
// $Id: faceted_search.inc,v 1.57 2009/01/04 19:36:25 davidlesieur Exp $

/**
 * @file
 * Provides base classes for implementing filters and facets, and classes needed
 * by other modules.
 */

/**
 * The base class for filters.
 *
 * Filters actually impact results only when they have an active category (a
 * "category" is a filtering value). The filtering is delegated to the active
 * category.
 */
class faceted_search_filter {

  /**
   * The key identifying this class of filter. Keys are used in the form of
   * 'key:text' tokens in the search text.
   */
  var $_key = '';

  /**
   * The status of this filter.
   */
  var $_status = FALSE;

  /**
   * The weight of this filter, for sorting purposes.
   */
  var $_weight = 0;

  /**
   * An array representing the path of categories leading to the active category
   * of this facet. This path includes the active category itself.
   */
  var $_path = array();

  /**
   * Constructor.
   *
   * @param $key
   *   Key corresponding to this class of filter. This should be the same string
   *   as used to construct the filter from the search text in the module's
   *   implementation of hook_faceted_search_parse().
   * @param $active_path
   *   Array representing the path leading to the active category, including the
   *   active category itself. Defaults to an empty array, meaning no active
   *   category.
   */
  function faceted_search_filter($key, $active_path = array()) {
    $this->_key = $key;
    $this->_path = $active_path;
  }

  /**
   * Return TRUE if this filter offers browsable categories, or FALSE otherwise.
   */
  function is_browsable() {
    return FALSE;
  }

  /**
   * Assign settings to this filter.
   *
   * @param $settings
   *   Array of settings.
   */
  function set($settings) {
    if (isset($settings['status'])) {
      $this->_status = $settings['status'];
    }
    if (isset($settings['weight'])) {
      $this->_weight = $settings['weight'];
    }
  }

  /**
   * Return the key for this class of filter.
   */
  function get_key() {
    return $this->_key;
  }

  /**
   * Return a help text for site administrators.
   */
  function get_help() {
    return '';
  }

  /**
   * Return the status of this filter.
   *
   * @return
   *   TRUE when the filter is enabled, FALSE otherwise.
   */
  function get_status() {
    return $this->_status;
  }

  /**
   * Change the status of this filter.
   *
   * @param $status
   *   TRUE to enable the filter, FALSE to disable it.
   */
  function set_status($status) {
    $this->_status = $status;
  }

  /**
   * Return the configured weight of this filter, for sorting purposes.
   */
  function get_weight() {
    return $this->_weight;
  }

  /**
   * Assign the weight of this filter.
   */
  function set_weight($weight) {
    $this->_weight = $weight;
  }

  /**
   * Return TRUE if this facet has an active category. If a facet is active, it
   * normally means that it is used in the current search.
   */
  function is_active() {
    return count($this->_path) > 0;
  }

  /**
   * Return an array representing the path to the active category, including the
   * active category itself. Return an empty array if there is no active
   * category.
   */
  function get_active_path() {
    return $this->_path;
  }

  /**
   * Set the path of the active category, including the active category itself.
   *
   * @param $path
   *   The path of the category (array of categories). Defaults to no active
   *   path.
   */
  function set_active_path($path = array()) {
    $this->_path = $path;
  }

  /**
   * Return the active category, or NULL if there is no active category.
   */
  function get_active_category() {
    return end($this->_path);
  }

  /**
   * Append keywords used by this filter into the specified array.
   */
  function get_keywords(&$keywords) {
    // Does nothing by default.
  }
}

/**
 * Base class for facet categories.
 */
class faceted_search_category {
  /**
   * The number of nodes associated to this category.
   */
  var $_count = NULL;

  /**
   * Constructor.
   *
   * @param $count
   *   The number of nodes associated to this category within the current
   *   search.
   */
  function faceted_search_category($count = NULL) {
    $this->_count = $count;
  }

  /**
   * Return the number of nodes associated to this category within the current
   * search.
   *
   * @return The number of matching nodes, or NULL is count is unknown.
   */
  function get_count() {
    return $this->_count;
  }

  /**
   * Return weight of this category, for sorting purposes.
   */
  function get_weight() {
    return 0;
  }

  /**
   * Updates a query for retrieving the subcategories of this category and their
   * associated nodes within the current search results.
   *
   * This only needs to be overridden for hierarchical facets.
   *
   * @param $query
   *   The query object to update.
   * @return
   *   FALSE if this facet can't have subcategories.
   */
  function build_subcategories_query(&$query) {
    return FALSE;
  }
}

/**
 * The parent class for facets.
 *
 * A facet is a filter with browsable categories.
 */
class faceted_search_facet extends faceted_search_filter {

  /**
   * The current sort criteria to use for this facet. This determines how to
   * sort the facet's categories.
   */
  var $_sort = 'count';

  /**
   * The maximum number of categories to show in this facet.
   */
  var $_max_categories = 10;

  /**
   * Constructor.
   *
   * @param $key
   *   Key corresponding to this class of facet. This should be the same string
   *   as used to construct the facet from the search text in the module's
   *   implementation of hook_faceted_search_parse().
   */
  function faceted_search_facet($key, $active_path = array()) {
    parent::faceted_search_filter($key, $active_path);
  }

  /**
   * Return TRUE if this filter offers browsable categories, or FALSE otherwise.
   *
   * A browsable filter implies that categories retrieval and sorting methods
   * are available.
   */
  function is_browsable() {
    return TRUE;
  }

  /**
   * Assign settings to this facet.
   *
   * @param $settings
   *   Array of settings.
   */
  function set($settings) {
    parent::set($settings);
    if (isset($settings['sort'])) {
      $this->_sort = $settings['sort'];
    }
    if (isset($settings['max_categories'])) {
      $this->_max_categories = $settings['max_categories'];
    }
  }

  /**
   * Return the available sort options for this facet. Each option is a key =>
   * label pair.
   *
   * Each key must have a corresponding handler method in the form
   * 'build_sort_query_key'.
   */
  function get_sort_options() {
    return array('count' => t('Count'));
  }

  /**
   * Return the current sort criteria for this facet.
   */
  function get_sort() {
    return $this->_sort;
  }

  /**
   * Assigns the current sort criteria for this facet.
   */
  function set_sort($sort) {
    // Assign value only if a corresponding handler exists.
    if (method_exists($this, 'build_sort_query_'. $sort)) {
      $this->_sort = $sort;
    }
  }

  /**
   * Handler for the 'count' sort criteria.
   */
  function build_sort_query_count(&$query) {
    $query->add_orderby('count', 'DESC');
  }

  /**
   * Applies the facet's current sort option to the given query.
   */
  function build_sort_query(&$query) {
    $method = 'build_sort_query_'. $this->_sort;
    if (method_exists($this, $method)) {
      $this->$method($query);
    }
  }

  /**
   * Return the configured maximum number of categories to show in this facet.
   *
   * @return
   *   The maximum number of categories, or 0 for no limit.
   */
  function get_max_categories() {
    return $this->_max_categories;
  }

  /**
   * Assign the maximum number of categories to show in this facet.
   *
   * @param $max_categories
   *   The maximum number of categories, or 0 for no limit.
   */
  function set_max_categories($max_categories) {
    $this->_max_categories = $max_categories;
  }

  /**
   * Updates a query for retrieving the root categories of this filter and their
   * associated nodes within the current search results.
   *
   * @param $query
   *   The query object to update.
   * @return
   *   FALSE if this filter can't have root categories.
   */
  function build_root_categories_query() {
    return FALSE;
  }

  /**
   * This factory method creates categories given query results that include the
   * fields selected in get_root_categories_query() or get_subcategories_query().
   *
   * @param $results
   *   $results A database query result resource.
   * @return
   *   Array of categories.
   */
  function build_categories($results) {
    return array();
  }

  /**
   * Inject components into the query for selecting nodes matching this facet's
   * active category.
   *
   * @param $query
   *   Query to inject the components into.
   * @param $words
   *   Array keyed by search index type, each element being an array of positive
   *   words to lookup for that index type. This method should insert any words
   *   it cares about.
   * @param $matches
   *   Minimum number of words that should match in query results for each index type.
   */
  function build_results_query(&$query, &$words, &$matches) {
    // Note: Facets ignore $words and $matches.
    if ($category = $this->get_active_category()) {
      $category->build_results_query($query);
    }
  }
}

/**
 * The base class of keyword categories.
 */
class faceted_search_keyword_category {

  /**
   * Append keywords used by this category into the specified array.
   */
  function get_keywords(&$keywords) {
    // Does nothing by default.
  }

  /**
   * Check whether a given word is allowed for searching.
   *
   * @return
   *   The allowed word, or NULL if it is not allowed.
   */
  function check_word($word) {
    if (is_numeric($word)) {
      return (int)ltrim($word, '-0');
    }
    return $word;
  }

  /**
   * Prepare a label for output.
   */
  function check_label($label, $html = FALSE) {
    if (!$html) {
      return strip_tags($label);
    }
    return $label;
  }
}

/**
 * The keyword AND category.
 */
class faceted_search_keyword_and_category extends faceted_search_keyword_category {
  var $_word = '';

  /**
   * Constructor.
   *
   * @param $phrase
   *   String containing the word to search.
   */
  function faceted_search_keyword_and_category($word) {
    $this->_word = $word;
  }

  /**
   * Return the label for this category.
   *
   * @param $html
   *   TRUE when HTML is allowed in the label, FALSE otherwise.
   */
  function get_label($html = FALSE) {
    return $this->check_label(theme('faceted_search_keyword_and_label', $this->_word), $html);
  }

  /**
   * Return the search text for this category.
   */
  function get_text() {
    return $this->_word;
  }

  /**
   * Append keywords used by this category into the specified array.
   */
  function get_keywords(&$keywords) {
    $keywords[] = $this->_word;
  }

  /**
   * Return the weight of this category, for sorting purposes.
   */
  function get_weight() {
    return 0;
  }

  /**
   * Inject components into the query for selecting nodes matching this category.
   *
   * @param $query
   *   Query to inject the components into.
   * @param $words
   *   Array keyed by search index type, each element being an array of positive
   *   words to lookup for that index type. This method should insert any words
   *   it cares about.
   * @param $matches
   *   Minimum number of words that should match in query results for each index type.
   * @param $type
   *   Type of search index entry to be searched.
   */
  function build_results_query(&$query, &$words, &$matches, $type) {
    if (($word = $this->check_word($this->_word)) && !isset($words[$type][$word])) {
      if (strlen($word) >= variable_get('minimum_word_size', 3)) {
        $words[$type][$word] = $word;
        $matches[$type]++;
      }
      else {
        // Short words are only searched against the dataset.
        $query->enable_part("{$type}_search_dataset");
        // Ensure this type will be searched even though it has no "long" word.
        if (!isset($words[$type])) {
          $words[$type] = array();
        }
      }

      // The dataset will have to be looked up as well if the query becomes more
      // complex because of other keyword search operators.
      $query->set_current_part("{$type}_search_dataset");
      $query->add_where("{$type}_search_dataset.data LIKE '%% %s %%'", $word);
      $query->set_current_part(); // Back to default part.
    }
  }
}

/**
 * The keyword phrase category.
 */
class faceted_search_keyword_phrase_category extends faceted_search_keyword_category {
  var $_phrase = '';

  /**
   * Constructor.
   *
   * @param $phrase
   *   String containing the phrase to search.
   */
  function faceted_search_keyword_phrase_category($phrase) {
    $this->_phrase = $phrase;
  }

  /**
   * Return the label for this category.
   *
   * @param $html
   *   TRUE when HTML is allowed in the label, FALSE otherwise.
   */
  function get_label($html = FALSE) {
    return $this->check_label(theme('faceted_search_keyword_phrase_label', $this->_phrase), $html);
  }

  /**
   * Return the search text for this operator.
   */
  function get_text() {
    return '"'. $this->_phrase .'"';
  }

  /**
   * Append keywords used by this category into the specified array.
   */
  function get_keywords(&$keywords) {
    $keywords[] = $this->_phrase;
  }

  /**
   * Return the weight of this category, for sorting purposes.
   */
  function get_weight() {
    return 1;
  }

  /**
   * Inject components into the query for selecting nodes matching this category.
   *
   * @param $query
   *   Query to inject the components into.
   * @param $words
   *   Array keyed by search index type, each element being an array of positive
   *   words to lookup for that index type. This method should insert any words
   *   it cares about.
   * @param $matches
   *   Minimum number of words that should match in query results for each index type.
   * @param $type
   *   Type of search index entry to be searched.
   */
  function build_results_query(&$query, &$words, &$matches, $type) {
    $split = explode(' ', $this->_phrase);
    foreach ($split as $word) {
      if ($word = $this->check_word($word)) {
        $words[$type][$word] = $word;
      }
    }
    if (count($split) > 0) {
      $matches[$type]++; // A phrase counts as one match.

      if (count($split) > 1) {
        // Real phrase. We'll have to verify it against the dataset.
        $query->enable_part("{$type}_search_dataset");
      }

      // Add phrase match conditions.
      $query->set_current_part("{$type}_search_dataset");
      $query->add_where("{$type}_search_dataset.data LIKE '%% %s %%'", $this->_phrase);
      $query->set_current_part(); // Back to default part.
    }
  }
}

/**
 * The keyword OR category.
 */
class faceted_search_keyword_or_category extends faceted_search_keyword_category {
  var $_words = array();

  /**
   * Constructor.
   *
   * @param $words
   *   Array containing the words to search.
   */
  function faceted_search_keyword_or_category($words) {
    $this->_words = $words;
  }

  /**
   * Return the label for this category.
   *
   * @param $html
   *   TRUE when HTML is allowed in the label, FALSE otherwise.
   */
  function get_label($html = FALSE) {
    return $this->check_label(theme('faceted_search_keyword_or_label', $this->_words), $html);
  }

  /**
   * Return the search text for this category.
   */
  function get_text() {
    return implode(' OR ', $this->_words);
  }

  /**
   * Append keywords used by this category into the specified array.
   */
  function get_keywords(&$keywords) {
    $keywords = array_merge($keywords, $this->_words);
  }

  /**
   * Return the weight of this category, for sorting purposes.
   */
  function get_weight() {
    return 2;
  }

  /**
   * Inject components into the query for selecting nodes matching this category.
   *
   * @param $query
   *   Query to inject the components into.
   * @param $words
   *   Array keyed by search index type, each element being an array of positive
   *   words to lookup for that index type. This method should insert any words
   *   it cares about.
   * @param $matches
   *   Minimum number of words that should match in query results for each index type.
   * @param $type
   *   Type of search index entry to be searched.
   */
  function build_results_query(&$query, &$words, &$matches, $type) {
    $where = '';
    $where_args = array();
    foreach ($this->_words as $word) {
      if (($word = $this->check_word($word)) && !isset($words[$type][$word])) {
        $words[$type][$word] = $word;
        if (!empty($where)) {
          $where .= ' OR ';
        }
        $where .= "{$type}_search_dataset.data LIKE '%% %s %%'";
        $where_args[] = $word;
      }
    }
    if (!empty($where)) {
      $matches[$type]++;

      // Matches will have to be checked against the dataset.
      $query->enable_part("{$type}_search_dataset");
      $query->set_current_part("{$type}_search_dataset");
      array_unshift($where_args, $where);
      call_user_func_array(array(&$query, 'add_where'), $where_args);
      $query->set_current_part(); // Back to default part.
    }
  }
}

/**
 * The keyword NOT category.
 */
class faceted_search_keyword_not_category extends faceted_search_keyword_category {
  var $_word = '';

  /**
   * Constructor.
   *
   * @param $word
   *   String containing the word to exclude from the search.
   */
  function faceted_search_keyword_not_category($word) {
    $this->_word = $word;
  }

  /**
   * Return the label for this category.
   *
   * @param $html
   *   TRUE when HTML is allowed in the label, FALSE otherwise.
   */
  function get_label($html = FALSE) {
    return $this->check_label(theme('faceted_search_keyword_not_label', $this->_word), $html);
  }

  /**
   * Return the search text for this operator.
   */
  function get_text() {
    return '-'. $this->_word;
  }

  /**
   * Return the weight of this category, for sorting purposes.
   */
  function get_weight() {
    return 3;
  }

  /**
   * Inject components into the query for selecting nodes matching this category.
   *
   * @param $query
   *   Query to inject the components into.
   * @param $words
   *   Array keyed by search index type, each element being an array of positive
   *   words to lookup for that index type. This method should insert any words
   *   it cares about.
   * @param $matches
   *   Minimum number of words that should match in query results for each index type.
   * @param $type
   *   Type of search index entry to be searched.
   */
  function build_results_query(&$query, &$words, &$matches, $type) {
    if ($word = $this->check_word($this->_word)) {
      // This is a negative word; do not insert it, but mark the type as used.
      if (!isset($words[$type])) {
        $words[$type] = array();
      }

      // Negative words are checked against the dataset.
      $query->enable_part("{$type}_search_dataset");
      $query->set_current_part("{$type}_search_dataset");
      $query->add_where("{$type}_search_dataset.data NOT LIKE '%% %s %%'", $word);
      $query->set_current_part(); // Back to default part.
    }
  }
}

/**
 * The filter for keyword search.
 *
 * Note: For keyword filters, the key corresponds to the type of search index
 * entry, and the id is always 'keyword'.
 */
class faceted_search_keyword_filter extends faceted_search_filter {
  var $_label = ''; // Label of the field.

  /**
   * Constructor.
   *
   * @param $type
   *   Type of the search index entries corresponding to the field.
   * @param $label
   *   Label of the field.
   * @param $category
   *   Active category of the field.
   */
  function faceted_search_keyword_filter($type, $label, $category = NULL) {
    parent::faceted_search_filter($type, isset($category) ? array($category) : array());
    $this->_label = $label;
  }

  /**
   * Returns the id of this filter.
   */
  function get_id() {
    return 'keyword';
  }

  /**
   * Return the search text corresponding to this filter.
   */
  function get_text() {
    if ($category = $this->get_active_category()) {
      return $category->get_text();
    }
    return '';
  }

  /**
   * Return the label of this filter. This method is responsible for ensuring
   * adequate security filtering.
   */
  function get_label() {
    return check_plain($this->_label);
  }

  /**
   * Append keywords used by this filter into the specified array.
   */
  function get_keywords(&$keywords) {
    if ($category = $this->get_active_category()) {
      $category->get_keywords($keywords);
    }
  }

  /**
   * Inject components into the query for selecting nodes matching this filter.
   *
   * @param $query
   *   Query to inject the components into.
   * @param $words
   *   Array keyed by search index type, each element being an array of positive
   *   words to lookup for that index type. This method should insert any words
   *   it cares about.
   * @param $matches
   *   Minimum number of words that should match in query results for each index type.
   */
  function build_results_query(&$query, &$words, &$matches) {
    if ($category = $this->get_active_category()) {
      $category->build_results_query($query, $words, $matches, $this->get_key());
    }
  }
}

/**
 * This class stores and processes data related to a search.
 */
class faceted_search {
  // TODO: Remove the '_' prefix from data members. These are not so convenient
  // for working with the schema.
  
  /**
   * The environment id for this search. Each search environment has its own
   * settings which make it possible to use multiple distinct search
   * interfaces. It is this id that allows to select the proper settings.
   */
  var $env_id = 0;

  /**
   * The full, unprocessed search text.
   */
  var $_text = '';

  /**
   * An array with all keywords found in the search text.
   */
  var $_keywords = array();

  /**
   * Name of the temporary results table. While it exists, this table can be
   * queried for various purposes, such as building the search interface.
   */
  var $_results_table = '';

  /**
   * Number of results in the results table. May be used only after a call to
   * execute().
   */
  var $_results_count = 0;

  /**
   * Flag to indicate whether the search has been executed.
   */
  var $_ready = FALSE;

  /**
   * Collection of filters currently used by this search.
   */
  var $_filters = array();

  /**
   * Constructor. Initialize the search environment.
   *
   * @param $record
   *   Optional for this environment, as fetched from the database. Defaults to
   *   NULL (for new environment).
   */
  function faceted_search($record = NULL) {
    // Assign default settings, ensuring that all "blanks" are properly filled.
    $this->init();

    if (isset($record)) {
      $this->init_from_record($record);
    }
  }

  /**
   * Initialize this search environment with default settings.
   */
  function init() {
    $this->name = '';
    $this->description = '';
    $this->settings['title'] = t('Search');
    $this->settings['ignore_status'] = FALSE;
    $this->settings['types'] = array();

    // Provide other modules an opportunity to add their own default settings.
    module_invoke_all('faceted_search_init', $this);
  }

  /**
   * Assign this search environment's settings from a record fetched from the
   * database. Existing settings will be overwritten only if they are present in
   * the record.
   *
   * @param $record
   *   Optional for this environment, as fetched from the database.
   */
  function init_from_record($record) {
    if (isset($record->settings)) {
      // The schema has this field serialized.
      $settings = unserialize($record->settings);

      if (is_array($settings)) {
        // Load the settings from the record while preserving any default
        // settings that are not present in the record.
        $this->settings = $settings + $this->settings;
      }
        
      unset($record->settings);
    }
      
    // Load the remaining data from the record.
    foreach ($record as $key => $value) {
      $this->$key = $value;
    }
  }
  
  /**
   * Return the original search text of this search (i.e. the text that was
   * passed to the constructor).
   */
  function get_text() {
    return $this->_text;
  }

  /**
   * Return an array with keywords used in the search.
   */
  function get_keywords() {
    return $this->_keywords;
  }

  /**
   * Return the filters used by this search.
   */
  function get_filters() {
    return $this->_filters;
  }

  /**
   * Return the specified filter.
   */
  function get_filter($index) {
    return $this->_filters[$index];
  }

  /**
   * Return the index of a filter given its key and id.
   */
  function get_filter_by_id($key, $id) {
    foreach ($this->_filters as $index => $filter) {
      if ($filter->get_key() == $key && $filter->get_id() == $id) {
        return array($index, $filter);
      }
    }
  }

  /**
   * Prepare the complete search environment (with its filters), parsing the
   * given search text. Requires that an env_id has been assigned previously.
   *
   * @param $text
   *   Optional search text. Defaults to the empty string.
   * @return
   *   TRUE is the search environment could be successfully built.
   */
  function prepare($text = '') {
    if (!$this->env_id) {
      return FALSE;
    }
    
    $this->_text = $text;
    $this->_results_table = 'temp_faceted_search_results_'. $this->env_id;
    
    // Load settings for all enabled filters in this search environment.
    $all_filter_settings = faceted_search_load_filter_settings($this);

    // Make a selection with all enabled filters.
    $selection = faceted_search_get_filter_selection($all_filter_settings);

    // Collect all filters relevant to this search.
    foreach (module_implements('faceted_search_collect') as $module) {
      $module_filters = array();
      $hook = $module .'_faceted_search_collect';

      // Parse the search text and obtain corresponding filters. Text is eaten as
      // it gets parsed.
      $text = $hook($module_filters, 'text', $this, $selection, $text);

      // Disallow filters that already have been collected from the search text.
      foreach ($module_filters as $filter) {
        unset($selection[$filter->get_key()][$filter->get_id()]);
      }

      // Collect any remaining allowed facets.
      if (!empty($selection)) {
        $hook($module_filters, 'facets', $this, $selection);
      }

      // Merge the filters listed by the current module.
      $this->_filters = array_merge($this->_filters, $module_filters);

      if (empty($selection)) {
        break; // No more filters allowed.
      }
    }

    // After filters have been collected, any remaining text is passed to the
    // node filters.
    faceted_search_collect_node_keyword_filters($this->_filters, 'text', $this, $text);

    // Prepare filters for use, assigning them their settings are sorting them.
    faceted_search_prepare_filters($this->_filters, $all_filter_settings);

    // Assign the keywords found.
    foreach ($this->_filters as $filter) {
      $filter->get_keywords($this->_keywords);
    }

    return TRUE;
  }

  /**
   * Return TRUE when the search has been executed.
   */
  function ready() {
    return $this->_ready;
  }

  /**
   * Perform the search and store the results in a temporary table.
   *
   * The prepare() method must have been called previously.
   *
   * Results are retrieved in two logical "passes". However, the two passes are
   * joined together into a single query.  And in the case of most simple
   * queries the second pass is not even used.
   *
   * The first pass selects a set of all possible matches (individual words
   * looked up in the search_index table), which has the benefit of also
   * providing the exact result set for simple "AND" or "OR" searches.
   *
   * The second portion of the query further refines this set by verifying
   * advanced text conditions, such negative or phrase matches (search text
   * checked against the search_dataset table).
   */
  function execute() {
    if (!$this->_filters) {
      return; // Nothing to search
    }

    $query = new faceted_search_query;
    if (!$this->settings['ignore_status'] || !user_access('administer nodes')) {
      // Restrict the search to published nodes only.
      $query->add_where('n.status = 1');
    }
    $query->add_groupby('n.nid');

    // Apply node type filter
    $types = faceted_search_types($this);
    if (!empty($types)) {
      $query->add_where("n.type IN ('". implode("','", $types) ."')");
    }

    // Inject keyword search conditions if applicable.
    $words = array(); // Positive words to include in the query.
    $matches = array();
    $word_score_expr = '';
    $word_score_arg = 0;
    foreach ($this->_filters as $filter) { // TODO: All filters are iterated; We should avoid iterating through those that are disabled.
      $filter->build_results_query($query, $words, $matches);
    }

    if (count($matches) > 0) {
      $query->add_having('COUNT(*) >= %d', max($matches));
    }

    // Some positive words were specified (and maybe some negatives as well).
    $words_where = array();
    $words_args = array();
    $words_scores = array();
    foreach ($words as $type => $type_words) {
      if (empty($type_words)) {
        // Negative words and/or short words were specified, but no positive
        // "long" words. Negative words and short words are looked up in
        // search_dataset, but since there are no positive "long" words, in this
        // particular case it is joined directly with the node table and we can
        // avoid joining search_index.
        $query->set_current_part("{$type}_search_dataset");
        $query->add_table('search_dataset', 'sid', 'n', 'nid', "{$type}_search_dataset");
        $query->add_where("{$type}_search_dataset.type = '%s'", $type);
        $query->set_current_part(); // Back to default part.
      }
      else {
        // Join the search index for the current index type.
        $query->add_table('search_index', 'sid', 'n', 'nid', "{$type}_search_index");

        // Join the search dataset for the current index type, in case we're
        // dealing with a complex query.
        $query->set_current_part("{$type}_search_dataset");
        $query->add_table('search_dataset', array('sid', 'type'), "{$type}_search_index", array('sid', 'type'), "{$type}_search_dataset");
        $query->set_current_part(); // Back to default part.

        $words_where[] = '('. substr(str_repeat("{$type}_search_index.word = '%s' OR ", count($type_words)), 0, -4) .") AND {$type}_search_index.type = '%s'";
        $words_args = array_merge($words_args, array_values($type_words));
        $words_args[] = $type;

        $query->add_table('search_total', 'word', "{$type}_search_index", 'word', "{$type}_search_total");

        $words_scores[] = "{$type}_search_index.score * {$type}_search_total.count";
      }
    }

    if (!empty($words_where)) {
      array_unshift($words_args, implode(' AND ', $words_where));
      call_user_func_array(array(&$query, 'add_where'), $words_args);
    }

    if (!empty($words_scores)) {
      // Add word score expression to the query.
      $score = 'SUM('. implode(' + ', $words_scores) .')';
      $query->set_current_part('normalize');
      $query->add_field(NULL, $score, 'score');
      $query->set_current_part();

      // Perform the word score normalization query.
      $query->enable_part('normalize');
      $normalize = db_result(db_query_range($query->query(), $query->args(), 0, 1));
      $query->disable_part('normalize');

      if (!$normalize) {
        $this->_ready = TRUE;
        return; // Return with no results.
      }

      $word_score_expr = '(%f * '. $score .')';
      $word_score_arg = 1.0 / $normalize;
    }

    // Add field needed for results.
    $query->add_field('n', 'nid', 'nid');

    // Add scoring expression to the query.
    $this->_add_scoring($query, $word_score_expr, $word_score_arg);

    // Give other modules an opportunity at altering the final query (e.g. for
    // additional filtering).
    module_invoke_all('faceted_search_query_alter', $this, $query);

    // Perform the search results query and store results in a temporary table.
    //
    // This is MySQL-specific. db_query_temporary() is not used because of the
    // need to specify the primary key. The index provides a huge performance
    // improvement.
    //
    // See http://drupal.org/node/109513 regarding the use of HEAP engine.
    db_query('CREATE TEMPORARY TABLE '. $this->_results_table .' (nid int unsigned NOT NULL, PRIMARY KEY (nid)) Engine=HEAP '. $query->query(), $query->args(), $this->_results_table);
    $this->_results_count = db_result(db_query('SELECT COUNT(*) FROM '. $this->_results_table));
    $this->_ready = TRUE;
  }

  /**
   * Fetch the items from the current search results, or from all available
   * nodes if no search text has been given.
   *
   * execute() must have been called beforehand.
   *
   * @return
   *   Array of objects with nid and score members.
   */
  function load_results($limit = 10) {
    $found_items = array();
    if ($this->_results_count) {
      $result = pager_query("SELECT * FROM ". $this->_results_table, $limit, 0, 'SELECT '. $this->_results_count);
      while ($item = db_fetch_object($result)) {
        $found_items[] = $item;
      }
    }
    return $found_items;
  }

  /**
   * Return the number of results for this search.
   *
   * execute() must have been called beforehand.
   */
  function get_results_count() {
    return $this->_results_count;
  }

  /**
   * Return the name of this search's (temporary) results table.
   */
  function get_results_table() {
    return $this->_results_table;
  }

  /**
   * Return the categories for the given facet and count matching nodes within
   * results.
   *
   * @param $facet
   *   The facet whose categories are to be loaded.
   * @param $from
   *   Ordinal number of the first category to load. Numbering starts at 0.
   * @param $max_count
   *   Number of categories to load.
   * @return
   *   Array of categories (objects having the faceted_search_category
   *   interface).
   */
  function load_categories($facet, $from = NULL, $max_count = NULL) {
    // Prepare the base query components to include the current search results
    // and to count nodes.
    $query = new faceted_search_query;
    $query->add_field(NULL, 'COUNT(DISTINCT(n.nid))', 'count');
    if (!$this->_ready) {
      // No temporary table available, search within all nodes.
      if (!$this->settings['ignore_status'] || !user_access('administer nodes')) {
        // Restrict the search to published nodes only.
        $query->add_where('n.status = 1');
      }

      // There is no results table at this point, so we can't rely on the
      // results table having been filtered already. Therefore, we ask modules
      // to alter the categories query instead.
      module_invoke_all('faceted_search_query_alter', $this, $query);
    }
    elseif ($this->_results_count > 0) {
      // Search within results.
      $query->add_table($this->_results_table, 'nid', 'n', 'nid', 'results', 'INNER', FALSE);
    }
    else {
      // Current search yields no results, thus no categories are possible.
      return array();
    }

    // Gather the query components that will retrieve the categories.
    if ($active_category = $facet->get_active_category()) {
      $has_categories = $active_category->build_subcategories_query($query);
    }
    else {
      $has_categories = $facet->build_root_categories_query($query);
    }
    if (!$has_categories) {
      return array();
    }

    // Apply sort criteria.
    $facet->build_sort_query($query);

    // Apply node type filter.
    $types = faceted_search_types($this);
    if (!empty($types)) {
      $query->add_where("n.type IN ('". implode("','", $types) ."')");
    }

    // Run the query and return the categories.
    if (isset($from) && isset($max_count)) {
      $results = db_query_range($query->query(), $query->args(), $from, $max_count);
    }
    else {
      $results = db_query($query->query(), $query->args());
    }
    return $facet->build_categories($results);
  }

  /**
   * Add scoring expression to the search query.
   */
  function _add_scoring(&$query, $word_score_expr = '', $word_score_arg = 0) {
    // Based on node_search() -- START

    $score_field = array();
    $score_arguments = array();
    if (!empty($word_score_expr) && $weight = (int)variable_get('node_rank_relevance', 5)) {
      $score_field[] = "%d * $word_score_expr";
      $score_arguments[] = $weight;
      $score_arguments[] = $word_score_arg;
    }
    if ($weight = (int)variable_get('node_rank_recent', 5)) {
      // Exponential decay with half-life of 6 months, starting at last indexed node
      $score_field[] = '%d * POW(2, (GREATEST(MAX(n.created), MAX(n.changed), MAX(c.last_comment_timestamp)) - %d) * 6.43e-8)';
      $score_arguments[] = $weight;
      $score_arguments[] = (int)variable_get('node_cron_last', 0);
      $query->add_table('node_comment_statistics', 'nid', 'n', 'nid', 'c', 'LEFT');
    }
    if (module_exists('comment') && $weight = (int)variable_get('node_rank_comments', 5)) {
      // Inverse law that maps the highest reply count on the site to 1 and 0 to 0.
      $scale = variable_get('node_cron_comments_scale', 0.0);
      $score_field[] = '%d * (2.0 - 2.0 / (1.0 + MAX(c.comment_count) * %f))';
      $score_arguments[] = $weight;
      $score_arguments[] = $scale;
      if (!$query->has_table('c')) {
        $query->add_table('node_comment_statistics', 'nid', 'n', 'nid', 'c', 'LEFT');
      }
    }
    // Based on node_search() -- END

    // Add the formulas and their arguments into the query.
    if (count($score_field)) {
      // Prepend the first three arguments for add_field().
      $score_arguments = array_merge(array(NULL, implode(' + ', $score_field), 'score'), $score_arguments);
      // Call $query->add_field() with all arguments.
      call_user_func_array(array(&$query, 'add_field'), $score_arguments);

      $query->add_orderby('score', 'DESC');
    }
  }
}

/**
 * This class allows to build SQL queries piece by piece.
 *
 * Query elements are assigned to parts. These parts may selectively enabled or
 * disabled to control the final assembled the SQL statements. This is useful
 * when some context is still unknown at the time the elements are gathered -
 * those elements can still be injected to the query object and later filtered
 * in or out depending on context.
 */
class faceted_search_query {
  var $primary_table_alias = '';
  var $table_queue = array(); // Ordered array of tables aliases to join.
  var $tables = array(); // Tables to join, keyed by their alias.
  var $fields = array(); // Fields, keyed by their alias.
  var $field_args = array();
  var $groupby = array();
  var $having = array();
  var $having_args = array();
  var $orderby = array();
  var $where = array();
  var $where_args = array();
  var $subqueries = array();
  var $subqueries_args = array();
  // Part to which query elements will be added to.
  var $current_part = 'default';
  // Parts enabled for use in the final assembled the query.
  var $parts = array('default' => 'default');

  /**
   * Constructor. Specifies the primary table and field for this query.
   *
   * The primary table and field are always assigned to the default part.
   */
  function faceted_search_query($primary_table = 'node', $primary_table_alias = 'n', $prefixing = TRUE) {
    $this->primary_table_alias = $primary_table_alias;
    $this->tables['default'][$primary_table_alias] = array(
      'table' => $primary_table,
      'field' => NULL,
      'left_table_alias' => NULL,
      'left_field' => NULL,
      'join' => NULL,
      'prefixing' => $prefixing,
    );
  }

  /**
   * Set the current part. This determines the part to which any query element
   * will be added to, until this method is called to select another part as
   * the current part.
   *
   * The current part cannot be "unset", but it can be reset back to the
   * default part.
   *
   * @param $part
   *   Name of the part. Defaults to 'default'.
   */
  function set_current_part($part = 'default') {
    $this->current_part = $part;
  }

  /**
   * Return the current part.
   */
  function get_current_part() {
    return $this->current_part;
  }

  /**
   * Mark a part as enabled for use in query assembling. The query() and args()
   * methods will only return query elements that belong to parts that have
   * been enabled.
   *
   * The default part is always enabled.
   *
   * @see query()
   * @see args()
   * @see disable_part()
   */
  function enable_part($part) {
    $this->parts[$part] = $part;
  }

  /**
   * Disallow a part for use in query assembling.
   *
   * The default part cannot be disabled.
   *
   * @see enable_part()
   */
  function disable_part($part) {
    if ($part != 'default') {
      unset($this->parts[$part]);
    }
  }

  /**
   * Indicate whether the specified part is enabled for use in query assembling.
   */
  function is_part_enabled($part) {
    return isset($this->parts[$part]);
  }

  /**
   * Add a table to join.
   *
   * @param $table
   *   Name of the table to join.
   * @param $field
   *   Field to use in the ON condition of the join clause. This can be an array
   *   if the condition involves multiple fields (multiple fields will be glued
   *   together with the AND operator).
   * @param $left_table_alias
   *   Alias of the table to use on the left part of the join. That table must
   *   be the query's primary table or another table added through
   *   add_table(). This must be an alias as returned by add_table().
   * @param $left_field
   *   Field from the left table to use in the ON condition of the join
   *   clause. If $field is array, then $left_field must be an array of the same
   *   length.
   * @param $alias
   *   Alias to use for the table being added. If unspecified, the alias will be
   *   the same as the table's name. A unique alias must be given if the table
   *   is to be joined multiple times.
   * @param $join
   *   Type of join clause to use. Default is 'INNER'.
   * @param $prefixing
   *   TRUE when the table should be prefixed via db_prefix_tables(). This
   *   should usually be FALSE when joining a temporary table.
   * @return
   *   The alias assigned to the table in this query.
   */
  function add_table($table, $field, $left_table_alias, $left_field, $alias = NULL, $join = 'INNER', $prefixing = TRUE) {
    $alias = $alias ? $alias : $table;
    $this->table_queue[$this->current_part][] = $alias;
    $this->tables[$this->current_part][$alias] = array(
      'table' => $table,
      'field' => $field,
      'left_table_alias' => $left_table_alias,
      'left_field' => $left_field,
      'join' => $join,
      'prefixing' => $prefixing,
    );
    return $alias;
  }

  /**
   * Indicate whether a table alias is present in this query.
   *
   * @param $alias
   *   Alias expected to have been assigned to a table in this query.
   * @param $part
   *   Optional. Part in which to look for the table. When not specified, the
   *   current part is used.
   * @return
   *   TRUE if the alias is present in the query, FALSE otherwise.
   */
  function has_table($alias, $part = NULL) {
    $part = isset($part) ? $part : $this->current_part;
    return isset($this->tables[$part][$alias]);
  }

  /**
   * Add a field.
   *
   * @param $table_alias
   *   Alias of the table containing the field, either the primary table or an
   *   alias returned by add_table(). Use NULL for a formula.
   * @param $field
   *   The name of the field, or the formula defining the field.
   * @param $alias
   *   Alias to use to identify the field. If omitted, the alias will be
   *   $table_alias .'_'. $field. Must be specified if the field is a formula.
   * @param ...
   *   A variable number of arguments which are substituted into the query using
   *   printf) syntax. The query arguments can be enclosed in one array
   *   instead. Valid %-modifiers are: %s, %d, %f, %b (binary data, do not
   *   enclose in '') and %%. This is useful when the field is defined by a
   *   formula.
   * @return
   *   The alias assigned to the field in this query.
   */
  function add_field($table_alias, $field, $alias = NULL) {
    $alias = $alias ? $alias : $table_alias .'_'. $field;
    $this->fields[$this->current_part][$alias] = array(
      'field' => $field,
      'table_alias' => $table_alias,
    );
    $args = func_get_args();
    array_shift($args); // Skip $table_alias.
    array_shift($args); // Skip $field.
    if (count($args)) {
      array_shift($args); // Skip $alias.
      if (count($args)) {
        // Add extra arguments.
        if (isset($this->field_args[$this->current_part])) {
          $this->field_args[$this->current_part] = array_merge($this->field_args[$this->current_part], $args);
        }
        else {
          $this->field_args[$this->current_part] = $args;
        }
      }
    }
    return $alias;
  }

  /**
   * Indicate whether a field alias is present in this query.
   *
   * @param $alias
   *   Alias expected to have been assigned to a field in this query.
   * @param $part
   *   Optional. Part in which to look for the field. When not specified, the
   *   current part is used.
   * @return
   *   TRUE if the alias is present in the query, FALSE otherwise.
   */
  function has_field($alias, $part = NULL) {
    $part = isset($part) ? $part : $this->current_part;
    return isset($this->fields[$part][$alias]);
  }

  /**
   * Add a WHERE condition. When the query is later assembled, all WHERE
   * conditions are glued together with the AND operator.
   *
   * @param $clause
   *   The condition to add. The caller must ensure that any field is fully
   *   qualified using its table's alias as returned by add_table().
   * @param ...
   *   A variable number of arguments which are substituted into the query using
   *   printf) syntax. The query arguments can be enclosed in one array
   *   instead. Valid %-modifiers are: %s, %d, %f, %b (binary data, do not
   *   enclose in '') and %%.
   */
  function add_where($clause) {
    $this->where[$this->current_part][] = $clause;
    $args = func_get_args();
    array_shift($args); // Skip $clause.
    if (count($args)) {
      if (isset($this->where_args[$this->current_part])) {
        $this->where_args[$this->current_part] = array_merge($this->where_args[$this->current_part], $args);
      }
      else {
        $this->where_args[$this->current_part] = $args;
      }
    }
  }

  /**
   * Add a subquery as a WHERE condition.
   *
   * @param $clause
   *   The condition to add. The caller must ensure that any field is fully
   *   qualified using its table's alias as returned by add_table(). The caller
   *   is responsible for calling db_rewrite_sql() on the subquery.
   * @param ...
   *   A variable number of arguments which are substituted into the query using
   *   printf) syntax. The query arguments can be enclosed in one array
   *   instead. Valid %-modifiers are: %s, %d, %f, %b (binary data, do not
   *   enclose in '') and %%.
   */
  function add_subquery($clause) {
    $this->subqueries[$this->current_part][] = $clause;
    $args = func_get_args();
    array_shift($args); // Skip $clause.
    if (isset($args[0]) && is_array($args[0])) {
      // Using the "all arguments in one array" syntax.
      $args = $args[0];
    }
    if (count($args)) {
      if (isset($this->subqueries_args[$this->current_part])) {
        $this->subqueries_args[$this->current_part] = array_merge($this->subqueries_args[$this->current_part], $args);
      }
      else {
        $this->subqueries_args[$this->current_part] = $args;
      }
    }
  }

  /**
   * Add a GROUP BY clause.
   *
   * @param $clause
   *   The clause to add. The caller must use field aliases as returned by
   *   add_field().
   * @param $order
   *   Either 'ASC' or 'DESC'.
   */
  function add_groupby($clause, $order = 'ASC') {
    $this->groupby[$this->current_part][] = $clause .' '. $order;
  }

  /**
   * Add a HAVING clause.
   *
   * @param $clause
   *   The clause to add. The caller must ensure that any field is fully
   *   qualified using its table's alias as returned by add_table().
   * @param ...
   *   A variable number of arguments which are substituted into the query using
   *   printf) syntax. The query arguments can be enclosed in one array
   *   instead. Valid %-modifiers are: %s, %d, %f, %b (binary data, do not
   *   enclose in '') and %%.
   */
  function add_having($clause) {
    $this->having[$this->current_part][] = $clause;
    $args = func_get_args();
    array_shift($args); // Skip $clause.
    if (count($args)) {
      if (isset($this->having_args[$this->current_part])) {
        $this->having_args[$this->current_part] = array_merge($this->having_args[$this->current_part], $args);
      }
      else {
        $this->having_args[$this->current_part] = $args;
      }
    }
  }

  /**
   * Add an ORDER BY clause.
   *
   * @param $clause
   *   The clause to add. The caller must use field aliases as returned by
   *   add_field().
   * @param $order
   *   Either 'ASC' or 'DESC'.
   */
  function add_orderby($clause, $order = 'ASC') {
    $this->orderby[$this->current_part][] = $clause .' '. $order;
  }

  /**
   * Return all arguments that need to be substituted into the query. Only
   * arguments associated to enabled parts are returned.
   *
   * @return
   *   Array of argument values to pass to the query.
   * @see query()
   * @see enable_part()
   */
  function args() {
    $field_args = array();
    $where_args = array();
    $subqueries_args = array();
    $having_args = array();
    // Where arguments.
    foreach ($this->parts as $part) {
      if (isset($this->field_args[$part])) {
        $field_args = array_merge($field_args, $this->field_args[$part]);
      }
      if (isset($this->where_args[$part])) {
        $where_args = array_merge($where_args, $this->where_args[$part]);
      }
      if (isset($this->subqueries_args[$part])) {
        $subqueries_args = array_merge($subqueries_args, $this->subqueries_args[$part]);
      }
      if (isset($this->having_args[$part])) {
        $having_args = array_merge($having_args, $this->having_args[$part]);
      }
    }
    return array_merge($field_args, $where_args, $subqueries_args, $having_args);
  }

  /**
   * Return the assembled SQL query (with unsubstituted arguments, if
   * any). Only query elements associated to enabled parts are used.
   *
   * @see args()
   * @see enable_part()
   */
  function query() {
    // Primary field.
    if ($this->tables['default'][$this->primary_table_alias]['prefixing']) {
      $wrapper_begin = '{';
      $wrapper_end = '}';
    }
    else {
      $wrapper_begin = $wrapper_end = '';
    }
    $primary = $wrapper_begin . $this->tables['default'][$this->primary_table_alias]['table'] . $wrapper_end .' AS '. $this->primary_table_alias;

    // Collect elements from all enabled parts.
    $fields = array();
    $joins = array();
    $where = array();
    $subqueries = array();
    $groupby = array();
    $having = array();
    $orderby = array();
    foreach ($this->parts as $part) {
      // Fields.
      if (isset($this->fields[$part])) {
        foreach ($this->fields[$part] as $field_alias => $field) {
          if ($field['table_alias']) {
            $table = $field['table_alias'];
            $fields[] = "$table.$field[field] AS $field_alias";
          }
          else {
            $fields[] = "$field[field] AS $field_alias";
          }
        }
      }

      // Joins.
      if (isset($this->table_queue[$part])) {
        foreach ($this->table_queue[$part] as $table_alias) {
          $table = $this->tables[$part][$table_alias];
          if ($table['prefixing']) {
            $wrapper_begin = '{';
            $wrapper_end = '}';
          }
          else {
            $wrapper_begin = $wrapper_end = '';
          }

          // Build the join condition.
          if (is_array($table['left_field'])) {
            // There are multiple fields to use in the join condition.
            $join_condition = array();
            foreach ($table['left_field'] as $index => $table_left_field) {
              $join_condition[] = "{$table['left_table_alias']}.{$table_left_field} = {$table_alias}.{$table['field'][$index]}";
            }
            $join_condition = implode(' AND ', $join_condition);
          }
          else {
            $join_condition = "{$table['left_table_alias']}.{$table['left_field']} = {$table_alias}.{$table['field']}";
          }

          // Add the table join clause.
          $joins[] = $table['join'] ." JOIN $wrapper_begin". $table['table'] ."$wrapper_end AS $table_alias ON $join_condition";
        }
      }

      // Where clauses.
      if (isset($this->where[$part])) {
        $where = array_merge($where, $this->where[$part]);
      }

      // Subqueries.
      if (isset($this->subqueries[$part])) {
        $subqueries = array_merge($subqueries, $this->subqueries[$part]);
      }

      // Group by clauses.
      if (isset($this->groupby[$part])) {
        $groupby = array_merge($groupby, $this->groupby[$part]);
      }

      // Having clauses.
      if (isset($this->having[$part])) {
        $having = array_merge($having, $this->having[$part]);
      }

      // Order by clauses.
      if (isset($this->orderby[$part])) {
        $orderby = array_merge($orderby, $this->orderby[$part]);
      }
    }

    $fields = implode(', ', $fields);
    $joins = count($joins) ? ' '. implode(' ', $joins) : '';
    $where = count($where) ? ' WHERE (('. implode(') AND (', $where) .'))' : '';

    // Where subqueries (added as $SUBQUERY$n tokens, which are replaced after the call
    // to db_rewrite_sql(). See related issue: http://drupal.org/node/151910).
    $subqueries_tokens = '';
    if (count($subqueries)) {
      $subqueries_tokens = (empty($where) ? ' WHERE ' : ' AND ') .'$SUBQUERY$'. implode(' AND $SUBQUERY$', array_keys($subqueries));
    }

    // Group by clauses.
    $groupby = count($groupby) ? ' GROUP BY '. implode(', ', $groupby) : '';

    // Having clauses.
    $having = count($having) ? ' HAVING ('. implode(') AND (', $having) .')' : '';

    // Order by clauses.
    $orderby = count($orderby) ? ' ORDER BY '. implode(', ', $orderby) : '';

    // Create the query string.
    $query = db_rewrite_sql("SELECT $fields FROM $primary$joins$where$subqueries_tokens$groupby$having$orderby");
    if (count($subqueries)) {
      foreach ($subqueries as $key => $subquery) {
        $search[] = '$SUBQUERY$'. $key;
        $replace[] = $subquery;
      }
      // Replace subquery tokens.
      $query = str_replace($search, $replace, $query);
    }

    return $query;
  }
}

