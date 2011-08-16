<?php
// $Id: relevant_content_cck.theme.inc,v 1.1.2.2 2010/11/17 17:53:01 njt1982 Exp $

/**
 * @file
 * Field theming functions
 */

function theme_relevant_content_cck_formatter_default($element) {
  return l($element['#item']['title'], 'node/'. $element['#item']['nid']);
}

function theme_relevant_content_cck_field_format_plain($element) {
  return check_plain($element['#item']['title']);
}

function theme_relevant_content_cck_field_format_teaser($element) {
  return node_view(node_load($element['#item']['nid'], TRUE));
}

function theme_relevant_content_cck_field_format_full($element) {
  return node_view(node_load($element['#item']['nid'], FALSE));
}

