<?php
// $Id: ds.views.inc,v 1.1.2.6 2011/02/02 18:40:18 swentel Exp $

/**
 * @file
 * Views hooks file.
 */

/**
 * Implements hook_views_plugins().
 */
function ds_views_plugins() {
  $path = drupal_get_path('module', 'ds');
  return array(
    'module' => 'ds',
    'row' => array(
      'ds' => array(
        'title' => t('Display suite entity'),
        'help' => t('Display the entity with the Display suite module.'),
        'handler' => 'views_plugin_ds_entity_view',
        'path' => $path .'/views',
        'base' => array('node', 'user', 'apachesolr'),
        'theme' => 'ds_row_entity',
        'uses options' => TRUE,
        'type' => 'normal',
      ),
    ),
  );
}
