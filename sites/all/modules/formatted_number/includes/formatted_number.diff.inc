<?php
// $Id: formatted_number.diff.inc,v 1.1.2.1 2008/12/07 21:06:10 markuspetrux Exp $

/**
 * @file
 * Integration of Formatted Number CCK field with diff module.
 */

/**
 * Implementation of hook_diff().
 */
function formatted_number_diff(&$old_node, &$new_node) {
  static $all_field_types;
  if (!isset($all_field_types)) {
    $all_field_types = array_keys(formatted_number_get_fields_info());
  }
  $result = array();
  $cck_info = content_types($new_node->type);
  if (isset($cck_info['fields']) && is_array($cck_info['fields'])) {
    foreach ($cck_info['fields'] as $field) {
      if (!in_array($field['type'], $all_field_types)) {
        continue;
      }
      $old_values = array();
      $new_values = array();
      if (isset($old_node->$field['field_name'])) {
        $old_values = _formatted_number_diff_values($old_node, $field);
      }
      if (isset($new_node->$field['field_name'])) {
        $new_values = _formatted_number_diff_values($new_node, $field);
      }
      $result[$field['field_name']] = array(
        '#name' => $field['widget']['label'],
        '#old' => $old_values,
        '#new' => $new_values,
        '#weight' => $field['widget']['weight'],
        '#format' => array(
          'show_header' => FALSE,
        ),
      );
    }
  }
  return $result;
}

function _formatted_number_diff_values($node, $field) {
  $result = array();
  foreach ($node->$field['field_name'] as $item) {
    $result[] = theme('formatted_number_formatter_default', array(
      '#field_name' => $field['field_name'],
      '#type_name' => $node->type,
      '#formatter' => 'default',
      '#item' => $item,
    ));
  }
  return $result;
}
