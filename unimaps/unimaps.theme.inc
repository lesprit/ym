<?php

/**
 * @file
 * Theming functions for the unimaps module.
 */

function theme_unimaps_formatter_default($element) {
  global $_unimaps_defaults;

  // we need to get default settings for this specific field.
  $field = content_fields($element['#field_name'], $element['#type_name']);
  $default_elements = array_keys($_unimaps_defaults);
  $defaults = array();
  foreach ($default_elements as $k) {
    $defaults[$k] = $field[$k];
  }

  return $element['#item']['safe'];
}

function theme_unimaps_formatter_latitude_only($element) {

  return check_plain($element['#item']['latitude']);
}

function theme_unimaps_formatter_longitude_only($element) {

  return check_plain($element['#item']['longitude']);
}

function theme_unimaps_formatter_yandex_icon_only($element) {

  return check_plain($element['#item']['yandex_icon']);
}

function theme_unimaps_formatter_google_icon_only($element) {

  return check_plain($element['#item']['google_icon']);
}

function theme_unimaps_formatter_yandex_maps($element) {
  global $_unimaps_defaults;

  // we need to get default settings for this specific field.
  $field = content_fields($element['#field_name'], $element['#type_name']);
  $default_elements = array_keys($_unimaps_defaults);
  $defaults = array();
  foreach ($default_elements as $k) {
    $defaults[$k] = $field[$k];
  }

  $data = unserialize($element['#item']['data']);

  if (user_access('view map content') && is_numeric($element['#item']['latitude']) && ($element['#item']['longitude'])) {

    $pl = array();
    $nid = 0;
    $pl[$nid]['title'] = truncate_utf8($element['#node']->title, 100, TRUE, TRUE);
    $pl[$nid]['description'] = truncate_utf8($element['#node']->body, 200, TRUE, TRUE);
    $pl[$nid]['lat'] = $element['#item']['latitude'];
    $pl[$nid]['lng'] = $element['#item']['longitude'];
    $pl[$nid]['drag'] = FALSE;

    if (!empty($element['#item']['yandex_icon'])) $pl[$nid]['icon'] = $element['#item']['yandex_icon'];

    $unimaps_map_options = array(
      'map_id' => '-unimaps-node-' . $element['#node']->nid . '-delta-' . $element['#item']['#delta'],
      'map_js' => 1,
      'map_type' => UNIMAPS_YMAP_TYPE,
      'map_zoom' => !empty($data['map_zoom']) ? $data['map_zoom'] : UNIMAPS_YMAP_ZOOM,
      'map_width' => !empty($data['map_width']) ? $data['map_width'] : UNIMAPS_MAP_WIDTH,
      'map_height' => !empty($data['map_height']) ? $data['map_height'] : UNIMAPS_MAP_HEIGHT,
      'zoom' => UNIMAPS_YMAP_ZOOMTYPE,
      'toolbar' => 1,
      'typecontrol' => UNIMAPS_YMAP_TYPECONTROL,
      'scaleline' => 1,
      'placemarks' => (count($pl) > 0) ? $pl : array(),
    );

    if (count($pl) == 0) {
      $unimaps_map_options['latitude'] = UNIMAPS_LATITUDE;
      $unimaps_map_options['longitude'] = UNIMAPS_LONGITUDE;
    }
    else {
      $unimaps_map_options['latitude'] = 0;
      $unimaps_map_options['longitude'] = 0;
    }

    // вызов hook_unimapsapi() с $op = 'options'
    $object->options = $unimaps_map_options;
    unimaps_invoke_unimapsapi($object, 'map', 'node');
    $unimaps_map_options = $object->options;

    _unimaps_show_map('yandex_maps', $unimaps_map_options);

    return '<div id="YMapsID' . $unimaps_map_options['map_id'] . '" style="width: ' . $unimaps_map_options['map_width'] . 'px; height: ' . $unimaps_map_options['map_height'] . 'px"></div>' . $help . $element['#children'];
  }
}

// Вывод карты в форме редактирования материала
function theme_unimaps($element) {  
  global $_unimaps_defaults;
  
  // we need to get default settings for this specific field.
  $field = content_fields($element['#field_name'], $element['#type_name']);
  $default_elements = array_keys($_unimaps_defaults);

  $defaults = array();

  foreach ($default_elements as $k) {
    $defaults[$k] = $field[$k];
  }

  $data = unserialize($element['#default_value']['data']);
  if (is_array($data)) $element['#default_value'] = array_merge($element['#default_value'], $data);
  
  $elements_name = $element['#field_name'] . '[' . $element['#delta'] . ']';

  if (is_numeric($element['#default_value']['latitude']) && ($element['#default_value']['longitude'])) {
    $pl = array();
    $nid = 0;
    $pl[$nid]['lat'] = $element['#default_value']['latitude'];
    $pl[$nid]['lng'] = $element['#default_value']['longitude'];
    $pl[$nid]['drag'] = user_access('edit placemark');
    if (!empty($element['#default_value']['yandex_icon'])) $pl[$nid]['icon'] = $element['#default_value']['yandex_icon'];
    $can_add = 0;
  }
  else $can_add = UNIMAPS_CAN_ADD_PLACEMARKS;

  $unimaps_map_options = array(
    'map_id' => '-unimaps-node-' . $element['#node']->nid . '-delta-' . $element['#item']['#delta'],
    'map_js' => 1,
    'map_type' => UNIMAPS_YMAP_TYPE,
    'map_zoom' => !empty($element['#default_value']['map_zoom']) ? $element['#default_value']['map_zoom'] : UNIMAPS_YMAP_ZOOM,
    'map_width' => !empty($element['#default_value']['map_width']) ? $element['#default_value']['map_width'] : UNIMAPS_MAP_WIDTH,
    'map_height' => !empty($element['#default_value']['map_height']) ? $element['#default_value']['map_height'] : UNIMAPS_MAP_HEIGHT,
    'zoom' => UNIMAPS_YMAP_ZOOMTYPE,
    'toolbar' => 0,
    'typecontrol' => UNIMAPS_YMAP_TYPECONTROL,
    'scaleline' => 0,
    'placemarks' => (count($pl) > 0) ? $pl : array(),
    'add' => user_access('add placemark'),
    'drag' => user_access('edit placemark'),
//    'click' => TRUE,
  );

  if (count($pl) == 0) {
    $unimaps_map_options['latitude'] = UNIMAPS_LATITUDE;
    $unimaps_map_options['longitude'] = UNIMAPS_LONGITUDE;
  }
  else {
    $unimaps_map_options['latitude'] = 0;
    $unimaps_map_options['longitude'] = 0;
  }

  $unimaps_map_options['lat_id'] = $element['latitude']['#id'];
  $unimaps_map_options['lng_id'] = $element['longitude']['#id'];
  $unimaps_map_options['zoom_id'] = $element['map_zoom']['#id'];

  $unimaps_map_options['can_add'] = $can_add;

  // вызов hook_unimapsapi() с $op = 'options'
  $object->options = $unimaps_map_options;
  unimaps_invoke_unimapsapi($object, 'map', 'node');
  $unimaps_map_options = $object->options;

  _unimaps_show_map('yandex_maps', $unimaps_map_options);

  $help = t("<p>Drag map by holding left button. Create marker by click.</p>");

  return '<div id="YMapsID' . $unimaps_map_options['map_id'] . '" style="width: ' . $unimaps_map_options['map_width'] . 'px; height: ' . $unimaps_map_options['map_height'] . 'px"></div>' . $help . $element['#children'];
}
