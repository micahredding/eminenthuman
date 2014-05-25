<?php
// $Id: content_complete.rules.inc,v 1.1.2.4.2.2 2010/10/13 14:06:59 pvhee Exp $

/**
 * @file
 * Some rules conditions/actions to check for completeness. 
 */
 
/**
 * Implements hook_rules_condition_info().
 * 
 * @ingroup rules
 * @see content_complete_completeness_percentage_form
 */
function content_complete_rules_condition_info() {
  $items['content_complete_completeness_percentage'] = array(
    'label' => t('Content Complete % is >= than x'),
    'parameter' => array(
      'node' => array(
        'type' => 'node', 
        'label' => t('Content')),
        
    ),
    'group' => t('Node'),
  );
  
  
  // $items['node_is_of_type']['parameter']['type'] = array(
  //   'type' => 'list<text>',
  //   'label' => t('Content types'),
  //   'options list' => 'node_type_get_names',
  //   'description' => t('The content type(s) to check for.'),
  //   'restriction' => 'input',
  // );
  
  $items['content_complete_completeness_percentage']['parameter']['percentage'] = array(
    'type' => 'textfield',
    'label' => t('% to compare'),
    'description' => t('The % to compare if >=. Example: 100, so the condition will be triggered if the content is complete.'),
  );
  
  return $items;
  
}


/**
 * Rules Condition form configuration - set the % to compare
 * 
 * @see content_complete_rules_condition_info
 */
function content_complete_completeness_percentage_form($settings = array(), &$form) {
  $form['settings']['percentage'] = array(
    '#type' => 'textfield',
    '#title' => t('% to compare'),
    '#default_value' => isset($settings['percentage']) ? $settings['percentage'] : '',
    '#description' => t('The % to compare if >=. Example: 100, so the condition will be triggered if the content is complete.')
  );
}

/**
 * Rules Condition - Content Complete % is >= than
 * 
 * @param $user 
 *   The user for which the condition is checked.
 * @param $settings
 *   The configured settings of the rule condition
 * 
 * @see content_complete_completeness_percentage_form
 * @see content_complete_rules_condition_info
 * 
 * @return TRUE or FALSE
 */
function content_complete_completeness_percentage($node, $settings) {
  $complete = content_complete_get_data($node);
  return ($complete['percent'] >= $settings['percentage']);
}