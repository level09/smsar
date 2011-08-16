<?php
// $Id: similarterms_handler_field_similar.inc,v 1.1.2.1 2010/09/09 13:58:19 jjeff Exp $

class similarterms_handler_field_similar extends views_handler_field {
  function option_definition() {
    $options = parent::option_definition();
    $options['count_type'] = array('default' => 1);
    $options['percent_suffix'] = array('default' => 1);
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['count_type'] = array(
      '#type' => 'radios',
      '#title' => t('Display type'),
      '#default_value' => $this->options['count_type'],
      '#options' => array(
        0 => t('Show count of common terms'),
        1 => t('Show as percentage'),
      ),
    );
    
    $form['percent_suffix'] = array(
      '#type' => 'checkbox',
      '#title' => t('Append % when showing percentage'),
      '#default_value' => !empty($this->options['percent_suffix']),
    );
        
  }

  function query() {
    // we MIGHT want to ensure that the COUNT data is getting added here.
  }

  function render($values) {
    if ($this->options['count_type'] == 0) {
      return $values->node_count;
    }
    elseif ($this->view->tids) {
      $output = round($values->node_count/count($this->view->tids) * 100);
      if (!empty($this->options['percent_suffix'])) {
        $output .= '%';
      }
      return $output;
    }
    
  }
}