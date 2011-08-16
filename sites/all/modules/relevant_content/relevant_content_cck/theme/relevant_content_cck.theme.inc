<?php
// $Id: relevant_content_cck.theme.inc,v 1.1.2.2 2009/09/17 12:09:53 njt1982 Exp $

/**
 * @file
 * Field theming functions specifically for Relevant Content CCK Fields
 */


function theme_relevant_content_cck_formatter_general($element, $type) {
  // If there are no items, return nothing. Anything else risks empty fields
  if (empty($element['items']['#item'])) return;

  // We only need this bit for token_* formatters
  if ($type == 'token_full' || $type == 'token_teaser') {
    // Get the field settings (for this node type)
    $field = content_fields($element['#field_name'], $element['#type_name']);

    // Get the token_full settings and make sure they're plain (this is important)
    $token_pattern = check_markup($field['widget'][$type]['body'], $field['widget'][$type]['format']);
  }

  // Init the output, count and num item variables
  $output = '';
  $count = 0;
  $num_items = count($element['items']['#item']);

  // Loop over all the items and create the field output
  foreach ($element['items']['#item'] as $item) {
    $attributes = array('class' => 'field-item ' . ($count % 2 ? 'even' : 'odd'));

    // Append a "first" class to the row
    if ($count == 0) {
      $attributes['class'] .= ' first';
    }

    // Append a "last" class to the row
    if ($count == $num_items - 1) {
      $attributes['class'] .= ' last';
    }

    // Depending on the type, render a field
    switch ($type) {
      default:
      case 'default' :
        $result = l($item['title'], 'node/'. $item['nid']);
        break;

      case 'plain' :
        $result = check_plain($item['title']);
        break;

      case 'teaser' :
      case 'full' :
        $result = node_view(node_load($item['nid']), ($type == 'teaser'));
        break;

      case 'token_full' :
      case 'token_teaser' :
        // Run a token replace on the content
        $types = array('global' => NULL, 'node' => node_load($item['nid']));
        $result = token_replace_multiple($token_pattern, $types);
        break;
    }

    // Generate the field
    $output .= '<div'. drupal_attributes($attributes) .'>'. $result .'</div>';
    $count++;
  }

  // Wrap the result in a field-items wrapper to make it content-field.tpl.php consistent
  return '<div class="field-items">'. $output .'</div>';

}



/**
 * Default formatter - hyperlinked nodes
 */
function theme_relevant_content_cck_formatter_default($element) {
  return theme('relevant_content_cck_formatter_general', $element, 'default');
}


/**
 * Optional formatter - plain text nodes
 */
function theme_relevant_content_cck_formatter_plain($element) {
  return theme('relevant_content_cck_formatter_general', $element, 'plain');
}


/**
 * Optional formatter - teaser nodes
 */
function theme_relevant_content_cck_formatter_teaser($element) {
  return theme('relevant_content_cck_formatter_general', $element, 'teaser');
}


/**
 * Optional formatter - full nodes
 */
function theme_relevant_content_cck_formatter_full($element) {
  return theme('relevant_content_cck_formatter_general', $element, 'full');
}


/**
 * Optional formatter - Token "teaser" field
 */
function theme_relevant_content_cck_formatter_token_teaser($element) {
  return theme('relevant_content_cck_formatter_general', $element, 'token_teaser');
}


/**
 * Optional formatter - Token "full" field
 */
function theme_relevant_content_cck_formatter_token_full($element) {
  return theme('relevant_content_cck_formatter_general', $element, 'token_full');
}
