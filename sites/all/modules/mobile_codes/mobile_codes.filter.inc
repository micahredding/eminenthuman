<?php
// $Id: mobile_codes.filter.inc,v 1.2 2009/02/26 00:53:51 deciphered Exp $

/**
 * @file
 * Mobile Codes filter integration
 *
 * Mobile Codes filter integration adds the ability to render a mobile code via
 * a text filter.
 */

/**
 * Implementation of hook_filter_tips().
 */
function mobile_codes_filter_tips($delta, $format, $long = FALSE) {
  if ($long) {
    return t('
      Create a Mobile Code using the following format:<br />
      [mobilecode type="<em>type</em>" data="<em>data</em>" size="<em>size</em>" name="<em>name</em>" tinyurl="<em>tinyurl</em>"]<em>content</em>[/mobilecode]
      <p>
        <strong>type</strong>: \'dm\' (Data Matrix), \'qr\' (QR Code) or leave blank for default.<br />
        <strong>data type</strong>: \'link\', \'text\', \'phone\' or leave blank for default.<br />
        <strong>size</strong>: \'small\', \'medium\', \'large\' or leave blank for default.<br />
        <strong>name</strong>: user defined (optional)<br />
        <strong>tinyurl</strong>: \'0\', \'1\' or leave blank for default.<br />
        <strong>content</strong>: user defined.
      </p>
    ');
  }

  else {
    return t('
      Create a Mobile Code using the following format:<br />
      [mobilecode type="<em>type</em>" data="<em>data type</em>" size="<em>size</em>" name="<em>name</em>" tinyurl="<em>tinyurl</em>"]<em>content</em>[/mobilecode]
    ');
  }
}

/**
 * Implementation of hook_filter().
 */
function mobile_codes_filter($op, $delta = 0, $format = -1, $text = '') {
  switch ($op) {
    case 'list':
      return array(0 => t('Mobile codes filter'));

    case 'description':
      return t('
        Create a Mobile Code using the following format:<br />
        [mobilecode type="<em>type</em>" data="<em>data type</em>" size="<em>size</em>" name="<em>name</em>" tinyurl="<em>tinyurl</em>"]<em>content</em>[/mobilecode]
      ');

    case 'prepare':
      return $text;

    case 'process':
      return mobile_codes_filter_process($text, $format);

    case 'settings':
      return mobile_codes_filter_settings($format);

    default:
      return $text;
  }
}

function mobile_codes_filter_process($text, $format) {
  $attr = '';

  if (preg_match_all('/\[mobilecode([^]]*)\]([^[]*)\[\/mobilecode\]/', $text, $codes, PREG_SET_ORDER)) {
    foreach ($codes as $match) {
      if (preg_match_all('/\s(\w+)="([^"]*)"/', $match[1], $attributes, PREG_SET_ORDER)) {
        foreach ($attributes as $attribute) {
          $attr[$attribute[1]] = $attribute[2];
        }
      }
      $text = str_replace($match[0], mobile_codes_generate($match[2], $attr, $format), $text);
      unset($attr);
    }
  }

  return $text;
}

function mobile_codes_filter_settings($format) {
  $form['mobile_codes_filter'] = array(
    '#type' => 'fieldset',
    '#title' => t('Mobile Codes filter'),
    '#collapsible' => TRUE,
  );
  $form['mobile_codes_filter'][] = mobile_codes_settings($format);

  return $form;
}
