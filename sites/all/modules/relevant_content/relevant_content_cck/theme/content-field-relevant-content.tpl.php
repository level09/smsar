<?php
// $Id: content-field-relevant-content.tpl.php,v 1.1.2.1 2009/09/15 16:18:03 njt1982 Exp $

/**
 * @file
 * This file overrides the default content-field.tpl.php file to allow us to do manual multiple fields.
 *
 * Available variables:
 * - $node: The node object.
 * - $field: The field array.
 * - $items: An array of values for each item in the field array.
 * - $teaser: Whether this is displayed as a teaser.
 * - $page: Whether this is displayed as a page.
 * - $field_name: The field name.
 * - $field_type: The field type.
 * - $field_name_css: The css-compatible field name.
 * - $field_type_css: The css-compatible field type.
 * - $label: The item label.
 * - $label_display: Position of label display, inline, above, or hidden.
 * - $field_empty: Whether the field has any valid value.
 * - $field_header: The render-safe output for the field header (if set)
 * - $field_items: The pre-rendered relevant items (from theme_relevant_content_cck_formatter_* functions)
 *
 * @see template_preprocess_field()
 */
?>
<?php if (!$field_empty) : ?>
<div class="field field-type-<?php print $field_type_css ?> field-<?php print $field_name_css ?>">
  <?php if ($label_display == 'above') : ?>
    <div class="field-label"><?php print t($label) ?>:&nbsp;</div>
  <?php endif;?>

  <?php if ($field_header) : ?>
    <div class="field-header"><?php print t($field_header); ?></div>
  <?php endif; ?>

  <?php print $field_items; ?>
</div>
<?php endif; ?>
