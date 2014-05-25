<?php
// $Id: theme.inc,v 1.1.2.8.2.2 2010/10/13 13:11:30 pvhee Exp $

/**
 * @file
 * Theme and template preprocessing code
 * 
 * TODO refactor inside preprocess functions
 */
 
/**
 * Block Theme function that displays the default output of a content
 * complete percent. Use this theme function to override
 * the output / display of this block.
 *
 * @param $complete
 *   Assoc array of
 *   ['type'] - str - The machine-readable name of this content type.
 *   ['name'] - str - The human-readable name of this content type.
 *   ['nid'] - int - The node ID of the node from this content type.
 *   ['percent'] - int - A number that represents the total percent complete (ie 50).
 *   ['leq_percent'] - int - A number that the current percent is lower or equal to (ie 25, 50, 75 or 100).
 *   ['completed'] - int - How many fields total that have been completed (filled out).
 *   ['incomplete'] - int - How many fields still need to be filled out.
 *   ['total'] - int - The count of all tagged profile fields.
 *   ['nextfield'] - str - The next field to fill out that is currently empty.
 *   ['nextpercent'] - int - What the total percent complete value will be when ['nextfield'] is complete.
 *   ['nextname'] - str - The field name of the next field for field focus.
 */
function theme_content_complete_profile_percent_complete($complete) {
  // add CSS for theming
  drupal_add_css(drupal_get_path('module', 'content_complete') .'/content_complete.css');
  $output = '<div id="content-complete-wrapper" class="content-complete-complete-wrapper">';
  
  if (isset($complete['nextpercent'])) {
    $output .= t('!complete% complete', array(
      '!complete' => $complete['percent'] 
    ));
  }
  
  // Divide percentages in 4 regions of 25 each
  $complete['leq_percent']=0;
  while ($complete['leq_percent']<=100) {
    if ($complete['percent'] <= $complete['leq_percent']) break;
    $complete['leq_percent']+=25;
  }
  
  if (!isset($complete['percent'])) {
    $complete['percent'] = 0; // protect against unset values
  }

  $output .= '<div id="content-complete-percent-bar-wrapper" class="content-complete-percent-bar-wrapper">';
  $output .= '<div id="content-complete-percent-bar" class="content-complete-percent-bar content-complete-percent-bar-leq-'. $complete['leq_percent'] .' content-complete-percent-bar-'. $complete['percent'] .'" style="width: '. $complete['percent'] .'%"></div>';
  $output .= '</div>';
  $output .= '</div>';

  if (isset($complete['edit']) && $complete['edit'] && isset($complete['nextfield']) && isset($complete['nextpercent'])) {
    $output .= t('Filling out <i>!nextfield</i> will bring !typename to !complete%.', array(
      '!nextfield' => l(t($complete['nextfield']), 'node/'. $complete['nid'] .'/edit', array('query' => array('content_complete_fieldname' => $complete['nextname']))),
      '!typename' => $complete['name'],
      '!complete' => $complete['nextpercent']
    ));
  }
  
  return $output;
}