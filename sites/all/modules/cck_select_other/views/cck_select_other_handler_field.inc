<?php
//$Id: cck_select_other_handler_field.inc,v 1.1.4.2 2010/11/08 03:19:42 mradcliffe Exp $
/**
 * @file
 */


/**
 * Extension to the Content Field Handler class for CCK Select Other
 */
class cck_select_other_handler_field extends views_handler_field_field {

  function render($values) {
    $entity_type = $values->_field_cache[$this->field_alias]['entity_type'];
    $field_name = $this->definition['field_name'];
    $object = $values->_field_cache[$this->field_alias]['object'];

    $instance = field_read_instance($entity_type, $field_name, $object->type);

    $options = cck_select_other_options($instance);
    $lang = $object->language;
    $value = $object->{$field_name}[$lang][0]['value'];

    return isset($options[$value]) ? $options[$value] : $value;
  }
}
