<?php
// $Id: similarterms_handler_sort_similar.inc,v 1.1.2.1 2010/09/09 13:58:19 jjeff Exp $

class similarterms_handler_sort_similar extends views_handler_sort {
  
  function option_definition() {
    $options = parent::option_definition();

    $options['order'] = array('default' => 'DESC');

    return $options;
  }

  function query() {
      
    // add function to count nid occurrences based on grouping 
    $this->query->add_field(NULL, 'COUNT(node.nid)', 'node_count', array('aggregate' => TRUE));
        
    // sort 'em
    $this->query->add_orderby(NULL, NULL, $this->options['order'], 'node_count');
    
    // group 'em
    $this->query->add_groupby('nid');
        
  }

}
