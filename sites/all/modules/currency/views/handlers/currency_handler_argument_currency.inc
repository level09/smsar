<?php

/**
 * @file
 *   Views argument handler.
 */

class currency_handler_argument_currency extends views_handler_argument {       
  function query() {      
    currency_views_preload($this->view);
    return parent::query();    
  } 

  /**
   * Get the title this argument will assign the view, given the argument.
   *
   * This usually needs to be overridden to provide a proper title.
   */
  function title() {                     
    $currency_names = currency_api_get_list();
    return $currency_names[$this->argument];
  }   
}
