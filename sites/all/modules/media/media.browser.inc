<?php
// $Id: media.browser.inc,v 1.30 2011/02/01 17:44:42 JacobSingh Exp $

/**
 * @file
 * Media Browser page callback
 */
function media_browser($selected = NULL) {
  $output = array();
  $output['#attached']['library'][] = array('media', 'media_browser_page');
  
  $params = drupal_get_query_parameters();
  array_walk_recursive($params, '_media_recursive_check_plain');
  media_set_browser_params($params);
  
  // How do we validate these?  I don't know.
  // I think PDO should protect them, but I'm not 100% certain.
  if (isset($params['fid'])) {
    // A form has submitted, selected a piece (or pieces) of media and we refreshed.
    // So we're just going to send the media entity out as a JS setting so the browser can close itself.
    $selected = $params['fid'];
    if (!is_array($params['fid'])) {
      $selected = array($params['fid']);
    }
    if (!is_numeric($selected[0])) {
      throw new Exception('Error selecting media, fid param is not an fid or an array of fids');
    }
    $selectedMedia = entity_load('media', $selected);
    foreach ($selectedMedia as &$media) {
      media_browser_build_media_item($media);
    }
    $setting = array('media' => array('selectedMedia' => array_values($selectedMedia)));
    drupal_add_js($setting, 'setting');
    return $output;
  }

  // Normal browser operation.
  foreach (module_implements('media_browser_plugin_info') as $module) {
    foreach(module_invoke($module, 'media_browser_plugin_info') as $key => $plugin_data) {
      $plugins[$key] = $plugin_data + array(
        '#module' => $module,
        '#weight' => 0,
      );
      $plugins[$key]['#weight'] += count($plugins)/1000;
    }
  }
  
  // Only the plugins in this array are loaded.
  if (!empty($params['enabledPlugins'])) {
    $plugins = array_intersect_key($plugins, array_fill_keys($params['enabledPlugins'], 1));
  }
  elseif (!empty($params['disabledPlugins'])) {
    $plugins = array_diff_key($plugins, array_fill_keys($params['disabledPlugins'], 1));
  }

  foreach ($plugins as $key => &$plugin) {
    $plugin += module_invoke($plugin['#module'], 'media_browser_plugin_view', $key, $params);
  }
  
  // Allow modules to change the tab names or whatever else they want to change
  // before we render.  Perhaps this should be an alter on the theming function
  // that we should write to be making the tabs.
  drupal_alter('media_browser_plugins', $plugins);
  
  $tabs = array(); // List of tabs to render.

  $settings = array('media' => array('browser' => array()));
  $browser_settings =& $settings['media']['browser'];

  //@todo: replace with Tabs module if it gets upgraded.
  foreach (element_children($plugins, TRUE) as $key) {
    $plugin =& $plugins[$key];

    //Add any JS settings
    $browser_settings[$key] = isset($plugin['#settings']) ? $plugin['#settings'] : array();

    // If this is a "ajax" style tab, add the href, otherwise an id.
    $href = isset($plugin['#callback']) ? $plugin['#callback'] : "#media-tab-$key";
    $tabs[] = "<a href='$href'><span>{$plugin['#title']}</span></a>";

    // Create a div for each tab's content.
    $plugin['#prefix'] = <<<EOS
    <div class="media-browser-tab" id="media-tab-$key">
EOS;
    $plugin['#suffix'] = <<<EOS
    </div>
    <!-- End #media-tab-$key -->
EOS;
  }

  drupal_add_js($settings, 'setting');

  $output['tabset'] = array(
    '#prefix' => '<div id="media-browser-tabset">',
    '#suffix' => '</div>',
  );
  $output['tabset']['list'] = array(
    '#markup' => '<ul><li>' . implode('</li><li>', $tabs) . '</li></ul>'
  );

  $output['tabset']['plugins'] = $plugins;
  return $output;
}

/**
 * Provides a singleton of the params passed to the media browser.
 *
 * This is useful in situations like form alters because callers can pass
 * id="wysiywg_form" or whatever they want, and a form alter could pick this up.
 * We may want to change the hook_media_browser_plugin_view() implementations to
 * use this function instead of being passed params for consistency.
 *
 * It also offers a chance for some meddler to meddle with them.
 *
 * @param array $params
 *  An array of parameters provided when a media_browser is launched.
 *  See media_browser().
 */
function media_set_browser_params(&$params = NULL) {
  static $stored_params;
  
  if (!$params) {
    return $stored_params;
  }
  $stored_params = $params;
  drupal_alter('media_browser_params', $stored_params);
}

/**
 * For sanity in grammar.
 * @see media_set_browser_params().
 */
function media_get_browser_params() {
  return media_set_browser_params();
}

/**
 * AJAX Callback function to return a list of media files
 */
function media_browser_list() {
  $params = drupal_get_query_parameters();
  // How do we validate these?  I don't know.
  // I think PDO should protect them, but I'm not 100% certain.
  array_walk_recursive($params, '_media_recursive_check_plain');
  
  $types = isset($params['types']) ? $params['types'] : NULL;
  $url_include_patterns = isset($params['url_include_patterns']) ? $params['url_include_patterns'] : NULL;
  $url_exclude_patterns = isset($params['url_exclude_patterns']) ? $params['url_exclude_patterns'] : NULL;

  $start = isset($params['start']) ? $params['start'] : 0;
  $limit = isset($params['limit']) ? $params['limit'] : media_variable_get('browser_pager_limit');
  
  $conditions = array();

  $query = new EntityFieldQuery();
  
  $query->range($start, $limit);
  $query->entityCondition('entity_type', 'media');
  $query->entityOrderBy('entity_id', 'DESC');

  if ($types) {
    $query->entityCondition('bundle', $types, 'IN');
  }

  if ($url_include_patterns) {
    $query->propertyCondition('uri', $v, 'CONTAINS');
    // Insert stream related restrictions here.
  }
  if ($url_exclude_patterns) {
    $query->propertyCondition('uri', "%$v%", 'NOT LIKE');
  }

  // @todo Implement granular editorial access: http://drupal.org/node/696970.
  //   In the meantime, protect information about private files from being
  //   discovered by unprivileged users. See also media_file_view().
  if (!user_access('administer media')) {
    $query->propertyCondition('uri', 'private://%', 'NOT LIKE');
  }

  $result = $query->execute();
  $ids = array_keys($result['media']);
  if ($ids) {
    $media_entities = entity_load('media', $ids);
  }
  else {
    $media_entities = array();
  }
  
  foreach ($media_entities as &$media) {
    media_browser_build_media_item($media);
  }
  
  drupal_json_output(array('media' => array_values($media_entities)));
  exit();

}

/**
 * Implements hook_media_browser_plugin_info().
 */
function media_media_browser_plugin_info() {
  $plugins = array();

  // @TODO: Should we add a permission around this?
  $plugins['library'] = array(
    '#weight' => 10,
  );

  if (user_access('administer media') || user_access('edit media')) {
    $plugins['upload'] = array(
      '#weight' => -10,
     );
  }
  return $plugins;
}

/**
 * Implementaion of hook_media_browser_plugin_view().
 */
function media_media_browser_plugin_view($plugin_name, $params) {
  $path = drupal_get_path('module', 'media');

  module_load_include('inc', 'media', 'media.admin');
  module_load_include('inc', 'media', 'media.pages');

  $types = isset($params['types']) ? $params['types'] : array();

  // The multiselect parameter is a string.  So we check to see if it is set and
  // adjust the local variable accordingly.
  $multiselect = FALSE;
  if (isset($params['multiselect']) && $params['multiselect'] != 'false') {
    $multiselect = TRUE;
  }

  $redirect = array('media/browser', array('query' => array('render' => 'media-popup')));
  switch ($plugin_name) {
    case 'upload':
      $attached = array();
      if ($multiselect && module_exists('plupload')) {
        $upload_form_id = 'media_add_upload_multiple';
        $attached['js'] = array($path . '/javascript/plugins/media.upload_multiple.js');
      }
      else {
        $upload_form_id = 'media_add_upload';
      }

      $upload_form = drupal_get_form($upload_form_id, $types);
      return array(
        '#title' => t('Upload'),
        'form' => array($upload_form),
        '#attached' => $attached,
      );
      break;
    case 'library':
      return array(
        '#title' => t('Library'),
        '#attached' => array(
          'js' => array(
            $path . '/javascript/plugins/media.library.js',
          ),
          'css' => array(
            //@todo: should move this.
            $path . '/javascript/plugins/media.library.css',
          ),
        ),
        '#settings' => array(
          'viewMode' => 'thumbnails',
          'getMediaUrl' => url('media/browser/list'),
          'types' => $types,
          'multiselect' => $multiselect,
        // We should probably change this to load dynamically when requested
        // via the JS file.
        ),
        '#markup' => '<div id="container"><div id="scrollbox"><ul id="media-browser-library-list" class="media-list-thumbnails"></ul><div id="status"></div></div></div>',
      );
    break;
  }

  return array();
 }

/**
 * Silly function to recursively run check_plain on an array.
 *
 * There is probably something in core I am not aware of that does this.
 *
 * @param $value
 * @param $key
 */
function _media_recursive_check_plain(&$value, $key) {
  $value = check_plain($value);
}

/**
 * Prepares the page to be able to launch the media browser.
 *
 * @TODO: This is a WTF at present.  Basically, the browser is launched from wysiwyg
 * and from fields, so having a common function makes sense.  But not like this.
 *
 * Defines default variables.
 */
function media_include_browser_js() {
  static $included;
  if ($included) {
    return;
  }
  $included = TRUE;
  drupal_add_library('media', 'media_browser');

  $settings = array(
    'browserUrl' => url('media/browser',
      array('query' => array('render' => 'media-popup'))),
    'styleSelectorUrl' => url('media/-media_id-/format-form',
      array('query' => array('render' => 'media-popup'))),

    // Adding src to blacklist; fid and view_mode we capture outside attribs so adding
    // them too to the blacklist.
    'blacklist' => array('src','fid','view_mode','format'), // Only applies to WYSIWG - should be removed;
  );

  drupal_add_js(array('media' => $settings), 'setting');
}


/**
 * Menu callback for testing the media browser
 */
function media_browser_testbed($form) {
  media_include_browser_js();

  $form['test_element'] = array(
    '#type' => 'media',
    '#media_options' => array(
      'global' => array(
        'types' => array('video', 'audio'),
      ),
    )
  );
  
  $launcher = '<a href="#" id="launcher"> Launch Media Browser</a>';

  $form['options'] = array(
    '#type' => 'textarea',
    '#title' => 'Options (JSON)',
    '#rows' => 10,
  );

  $form['launcher'] = array(
    '#markup' => $launcher,
  );

  $form['result'] = array(
    '#type' => 'textarea',
    '#title' => 'Result',
  );


  $js = <<<EOF
    Drupal.behaviors.mediaTest = {
    attach: function(context, settings) {
      var delim = "---";
      var recentOptions = [];
      var recentOptionsCookie = jQuery.cookie("recentOptions");
      if (recentOptionsCookie) {
        recentOptions = recentOptionsCookie.split("---");
      }

      var recentSelectBox = jQuery('<select id="recent_options" style="width:100%"></select>').change(function() { jQuery('#edit-options').val(jQuery(this).val())});

      jQuery('.form-item-options').append('<label for="recent_options">Recent</a>');
      jQuery('.form-item-options').append(recentSelectBox);
      jQuery('.form-item-options').append(jQuery('<a href="#">Reset</a>').click(function() {alert('reset'); jQuery.cookie("recentOptions", null); window.location.reload(); }));

      jQuery.each(recentOptions, function (idx, val) {
        recentSelectBox.append(jQuery('<option></option>').val(val).html(val));
      });


      jQuery('#launcher').click(function () {
        jQuery('#edit-result').val('');
        var options = {};
        var optionsTxt = jQuery('#edit-options').val();
        if (optionsTxt) {
          // Store it in the recent box
          recentOptionsCookie += "---" + optionsTxt
          jQuery.cookie("recentOptions", recentOptionsCookie, { expires: 7 });
          recentSelectBox.append(jQuery('<option></option>').val(optionsTxt).html(optionsTxt));
          options = eval('(' + optionsTxt + ')');
        }
        Drupal.media.popups.mediaBrowser(Drupal.behaviors.mediaTest.mediaSelected, options);
        return false;
      });
    },

    mediaSelected: function(selectedMedia) {
      var result = JSON.stringify(selectedMedia);
        jQuery('#edit-result').val(result);
    }
  }

EOF;

  drupal_add_js($js, array('type' => 'inline'));
  return $form;
}

/**
 * Adds on properties to the media item which are expected by callers who are
 * going to act on the returned item.
 *
 * @param Object &$media;
 */
function media_browser_build_media_item(&$media) {
  $preview = media_get_thumbnail_preview($media);
  $media->preview = drupal_render($preview);
  $media->url = file_create_url($media->uri);
}
