<?php
// $Id: formatted_number.token.inc,v 1.1.2.1 2009/08/16 08:55:30 markuspetrux Exp $

/**
 * @file
 * Integration of Formatted Number CCK field with token module.
 */

/**
 * Implementation of hook_token_list().
 */
function formatted_number_token_list($type = 'all') {
  if ($type == 'field' || $type == 'all') {
    return array(
      'formatted number' => array(
        'raw' => t('Raw number value'),
        'formatted' => t('Formatted number value'),
      ),
    );
  }
}

/**
 * Implementation of hook_token_values().
 */
function formatted_number_token_values($type, $items = NULL) {
  if ($type == 'field') {
    return array(
      'raw' => (isset($items[0]['value']) ? $items[0]['value'] : ''),
      'formatted' => (isset($items[0]['view']) ? $items[0]['view'] : ''),
    );
  }
}
