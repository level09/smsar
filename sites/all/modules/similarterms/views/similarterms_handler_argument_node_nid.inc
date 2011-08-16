<?php
// $Id $
/**
 * @file
 * Provide node nid argument handler.
 */

/**
 * Argument handler to accept a node id.
 * based on node_handler_argument_node_nid except that it doesn't
 * add a where clause to the query
 */
class similarterms_handler_argument_node_nid extends views_handler_argument_numeric {
  /**
   * Override the behavior of title(). Get the title of the node.
   */
  function title_query() {
    $titles = array();
    $placeholders = implode(', ', array_fill(0, sizeof($this->value), '%d'));

    $result = db_query("SELECT n.title FROM {node} n WHERE n.nid IN ($placeholders)", $this->value);
    while ($term = db_fetch_object($result)) {
      $titles[] = check_plain($term->title);
    }
    return $titles;
  }
  
  function option_definition() {
    $options = parent::option_definition();

    $options['vocabularies'] = array('default' => array());
    $options['include_args'] = array('default' => FALSE);
    
    return $options;
  }
  
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    
    unset($form['not']);
    
    $r = db_query('SELECT vid, name FROM {vocabulary} ORDER BY weight');
    while ($row = db_fetch_object($r)) {
      $vocabs[$row->vid] = $row->name;
    }
    
    $form['vocabularies'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Limit similarity to terms within these vocabularies'),
      '#description' => t('Choosing any vocabularies here will limit the terms used to calculate similarity. It is usually best NOT to limit the terms, but in some cases this is necessary. Leave all checkboxes unselected to not limit terms.'),
      '#options' => $vocabs,
      '#default_value' => empty($this->options['vocabularies']) ? array() : $this->options['vocabularies'],
    );
    
    $form['include_args'] = array(
      '#type' => 'checkbox',
      '#title' => t('Include argument node(s) in results'),
      '#description' => t('If selected, the node(s) passed as the argument will be included in the view results.'),
      '#default_value' => !empty($this->options['include_args']),
    );

  }
  
  function validate_arg($arg) {
    
    // first run the inherited arg validation
    if (!parent::validate_arg($arg)) {
      return FALSE;
    }
  
    // hmmm... @todo: wildcard validation?
    // see views_handler_argument.inc for possible code
  
    if (!empty($this->options['break_phrase'])) {
      views_break_phrase($this->argument, $this);
    }
    else {
      $this->value = array($this->argument);
    }
        
    // $vids is array node version ids
    $vids = array();
    foreach($this->value as $nid) {
      // get the current revision id (vid) for this node id (nid)
      $vids[] = db_result(db_query("SELECT vid FROM {node} WHERE nid = %d", $nid));
    }
        
    // $vocabs is array of vocabulary ids (a.k.a. vids, confusing right?)
    $vocabs = empty($this->options['vocabularies']) ? array() : $this->options['vocabularies'];
    foreach ($vocabs as $key => $val) {
      if ($val == 0) {
        unset($vocabs[$key]);
      }
    }
        
    $addwhere = '';
    $addjoin = '';
    if (count($vocabs) == 1) {
      // we're limiting the terms to those of given vocabs
      $addjoin = ' INNER JOIN {term_data} td ON tn.tid = td.tid ';
      $addwhere = " AND td.vid = %d";
    }
    elseif (count($vocabs) > 1) {
      $addjoin = ' INNER JOIN {term_data} td ON tn.tid = td.tid ';
      $placeholders = implode(', ', array_fill(0, sizeof($vocabs), '%d'));
      $addwhere = " AND td.vid IN ($placeholders)";
    }
    
    $args = array_merge($vids, $vocabs);
    if (count($vids) > 1) {
      $placeholders = implode(', ', array_fill(0, sizeof($vids), '%d'));
      $result = db_query("SELECT tn.tid FROM {term_node} tn $addjoin WHERE tn.vid IN ($placeholders) $addwhere", $args);
    }
    else {
      $result = db_query("SELECT tn.tid FROM {term_node} tn $addjoin WHERE tn.vid = %d $addwhere", $args);
    }
    
    $tids = array();
    while ($row = db_fetch_object($result)) {
      // adding a key to ensure there aren't duplicates
      $tids[$row->tid] = $row->tid;
    }
    
    $this->tids = $tids;
    $this->view->tids = $tids;
    
    if (count($tids) == 0) {
      // there are no terms...
      // we need to cancel the query and bail out
      return FALSE;
    }
    
    return TRUE;
    
  }
  
  
  function query() {
  
    $this->ensure_my_table();
        
    $tids = $this->tids;
  
    $v = $this->query->add_table('term_node');
              
    if (count($tids) == 1) {
      $this->query->add_where(0, "term_node.tid = %d", $tids);
    }
    elseif (count($tids) > 1) {
      $placeholders = implode(', ', array_fill(0, count($tids), '%d'));
      $this->query->add_where(0, "term_node.tid IN ($placeholders)", $tids);
    }
          
    // exclude the current node(s)
    if (empty($this->options['include_args'])) {
      if (count($this->value) > 1) {
        $placeholders = implode(', ', array_fill(0, count($this->value), '%d'));
        $this->query->add_where(0, "node.nid NOT IN ($placeholders)", $this->value);
      }
      else {
        $this->query->add_where(0, 'node.nid != %d', $this->value[0]);
      }
    }
    
  }
  
}