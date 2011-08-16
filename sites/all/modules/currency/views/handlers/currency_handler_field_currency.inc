<?php 

/**
 * @file
 *   Views field handler.
 */

class currency_handler_field_currency extends views_handler_field {
  function render($values) {  
    if ($this->options['display_code_only']) {
      return parent::render($values);
    }
    else {                 
      $currency_names = currency_api_get_list();
      $value = $currency_names[$values->{$this->field_alias}];
      return check_plain($value);
    }
  }
    
  function option_definition() {
    $options = parent::option_definition();
    $options['display_code_only'] = array('default' => FALSE);
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['display_code_only'] = array(
      '#title' => t('Display as currency code only'),
      '#description' => t('Hides the descriptive currency name.'),
      '#type' => 'checkbox',
      '#default_value' => !empty($this->options['display_code_only']),
    );
  }  
}
