<?php
// $Id: field-relevant-content.tpl.php,v 1.3 2009/10/29 14:31:10 njt1982 Exp $

/**
 * @file field-relevant-content.tpl.php
 * Relevant Content specific theme implementation (based around the default field.tpl.php) to display the value of a field.
 *
 * Available variables:
 * - $header: The optional header for the field.
 * - $items: An array of field values. Use render() to output them.
 * - $label: The item label.
 * - $label_hidden: Whether the label display is set to 'hidden'.
 * - $classes: String of classes that can be used to style contextually through
 *   CSS. It can be manipulated through the variable $classes_array from
 *   preprocess functions. The default values can be one or more of the
 *   following:
 *   - field-name-[field_name]: The current field name. For example, if the
 *     field name is "field_description" it would result in
 *     "field-name-field-description".
 *   - field-type-[field_type]: The current field type. For example, if the
 *     field type is "text" it would result in "field-type-text".
 *   - field-label-[label_display]: The current label position. For example, if the
 *     label position is "above" it would result in "field-label-above".
 *
 * Other variables:
 * - $object: The object to which the field is attached.
 * - $field: The field array.
 * - $build_mode: Build mode, e.g. 'full', 'teaser'...
 * - $field_name: The field name.
 * - $field_type: The field type.
 * - $field_name_css: The css-compatible field name.
 * - $field_type_css: The css-compatible field type.
 * - $field_language: The field language.
 * - $field_translatable: Whether the field is translatable or not.
 * - $label_display: Position of label display, inline, above, or hidden.
 * - $classes_array: Array of html class attribute values. It is flattened
 *   into a string within the variable $classes.
 *
 * @see template_preprocess_field()
 */
?>
<?php if ($items) : ?>
  <div class="field <?php print $classes; ?> clearfix"<?php print $attributes; ?>>
    <?php if (!$label_hidden) : ?>
      <div class="field-label"<?php print $title_attributes; ?>><?php print $label ?>:&nbsp;</div>
    <?php endif; ?>
    <div class="field-content">
    <?php if ($header) : ?>
    <div class="field-header">
      <?php print $header ?>
    </div>
    <?php endif; ?>
    <?php print render($items); ?>
    </div>
  </div>
<?php endif; ?>
