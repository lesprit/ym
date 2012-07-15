<?php

/**
 * @file
 * Field support functions for the unimaps module.
 */

/**
 * Implements hook_field_info().
 */
function unimaps_field_info() {
  return array(
    'unimaps_latlng' => array(
      'label' => t('Unimaps'),
      'description' => t('Unimaps input.'),
      'default_widget' => 'unimaps_yandex',
      'default_formatter' => 'unimaps_yandex',
    ),
  );
}

/**
 * Implements hook_field_validate().
 */
function unimaps_field_validate($entity_type, $entity, $field, $instance, $langcode, $items, &$errors) {

  foreach ($items as $delta => $item) {

    if (unimaps_field_is_empty($item, $field)) {
      $errors[$field['field_name']][$langcode][$delta][] = array(
        'error' => 'unimaps_no_geoobjects',
        'message' => t('Empty map. You must place geoobject.'),
      );
    }
    else {
      switch (TRUE) {
        case !is_numeric($item['lat']):
          $errors[$field['field_name']][$langcode][$delta][] = array(
            'error' => 'unimaps_invalid_lat',
            'message' => t('Invalid Latitude. Value must be numeric.'),
          );
          break;
        case $item['lat'] > 90:
        case $item['lat'] < -90:
          $errors[$field['field_name']][$langcode][$delta][] = array(
            'error' => 'unimaps_invalid_lat',
            'message' => t('Invalid Latitude. Value must be between 90 and -90.'),
          );
          break;
      }

      switch (TRUE) {
        case !is_numeric($item['lng']):
          $errors[$field['field_name']][$langcode][$delta][] = array(
            'error' => 'unimaps_invalid_lng',
            'message' => t('Invalid Longitude. Value must be numeric.'),
          );
        case $item['lng'] > 180:
        case $item['lng'] < -180:
          $errors[$field['field_name']][$langcode][$delta][] = array(
            'error' => 'unimaps_invalid_lng',
            'message' => t('Invalid Longitude. Value must be between 180 and -180'),
          );
          break;
      }
    }
  }
}

/**
 * Implements hook_field_is_empty().
 */
function unimaps_field_is_empty($item, $field) {
  if (empty($item['lat'])) return TRUE;
  if (empty($item['lng'])) return TRUE;
}

/**
 * Implements hook_field_formatter_info().
 */
function unimaps_field_formatter_info() {
  return array(
    'unimaps_text' => array(
      'label' => t('Lat/Lng formatter'),
      'field types' => array('unimaps_latlng'),
    ),
    'unimaps_latitude' => array(
      'label' => t('Latitude only formatter'),
      'field types' => array('unimaps_latlng'),
    ),
    'unimaps_longitude' => array(
      'label' => t('Longitude only formatter'),
      'field types' => array('unimaps_latlng'),
    ),
    'unimaps_name' => array(
      'label' => t('Name only formatter'),
      'field types' => array('unimaps_latlng'),
    ),
    'unimaps_descr' => array(
      'label' => t('Description only formatter'),
      'field types' => array('unimaps_latlng'),
    ),
    'unimaps_yandex_icon' => array(
      'label' => t('Yandex icon formatter'),
      'field types' => array('unimaps_latlng'),
    ),
    'unimaps_yandex' => array(
      'label' => t('Yandex maps formatter'),
      'field types' => array('unimaps_latlng'),
    ),
  );
}

/**
 * Implements hook_field_formatter_view().
 */
function unimaps_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {

  $element = array();

  switch ($display['type']) {
    case 'unimaps_yandex':

      foreach ($items as $delta => $item) {

        $name = isset($items[$delta]['name']) ? $items[$delta]['name'] : '';
        $descr = isset($items[$delta]['descr']) ? $items[$delta]['descr'] : array();
        $lat = isset($items[$delta]['lat']) ? $items[$delta]['lat'] : UNIMAPS_LATITUDE;
        $lng = isset($items[$delta]['lng']) ? $items[$delta]['lng'] : UNIMAPS_LONGITUDE;
        $yandex_icon = isset($items[$delta]['yandex_icon']) ? $items[$delta]['yandex_icon'] : (variable_get('unimaps_yandex_api', 2) ? UNIMAPS_YANDEX2_ICON : UNIMAPS_YANDEX_ICON);
        $data = isset($items[$delta]['data']) ? unserialize($items[$delta]['data']) : array();
        $zoom = isset($data['zoom']) ? $data['zoom'] : UNIMAPS_YMAP_ZOOM;

        // Извлечение всех дополнительных параметров метки из поля 'data'
        $zoom_type = isset($data['options']['zoom_type']) ? $data['options']['zoom_type'] : UNIMAPS_YMAP_ZOOM;
        $zoom_type = isset($options['zoom_type']) ? $options['zoom_type'] : UNIMAPS_YMAP_ZOOMTYPE;
        $type = isset($data['type']) ? $data['type'] : UNIMAPS_YMAP_TYPE;
        $height = isset($data['options']['height']) ? $data['options']['height'] : UNIMAPS_MAP_HEIGHT;
        $width = isset($data['options']['width']) ? $data['options']['width'] : UNIMAPS_MAP_WIDTH;

        if (module_exists('unimaps_yandex_shapes') && !empty($data['polyline'])) {
          $polyline = $data['polyline'];
          if (isset($polyline['points'])) $points = unimaps_coords_decode($polyline['points']);
        }
        if (module_exists('unimaps_yandex_shapes') && !empty($data['polygon'])) {
          $polygon = $data['polygon'];
          if (isset($polygon['points'])) $points = unimaps_coords_decode($polygon['points']);
        }

        if (isset($data['options'])) {
          $options = $data['options'];
          foreach ($options['controls'] as $k => $v) {
            if ($v == '0') unset($options['controls'][$k]);
          }
          foreach ($options['behaviors'] as $k => $v) {
            if ($v == '0') unset($options['behaviors'][$k]);
          }
        }

        $map_id = isset($items[$delta]['map_id']) ? 'unimaps-' . $items[$delta]['map_id'] : 'unimaps-node-' . $entity->nid . '-' . $delta;

          $params = array(
            'map_id' => $map_id,
            'map_delta' => $delta,
            'latitude' => $lat,
            'longitude' => $lng,
            'map_height' => $height,
            'map_width' => $width,
            'map_type' => $type,
            'map_zoom' => $zoom,
            'zoom' => ($zoom_type == 'NONE') ? '' : $zoom_type,
            'add' => TRUE,
            'add_icon' => $yandex_icon,
            'add_title' => $name,
            'add_descr' => is_array($descr) ? $descr['value'] : $descr,
            'add_cust_icon' => '',
            'can_add' => 0,
          );

          // выводить только для линий
          if (module_exists('unimaps_yandex_shapes') && user_access('view placemark') && $entity->type == 'ym_polyline') {
            $style = 'lines';
            $params[$style][0] = array(
              'points' => $points,
              'ym_pl_name' => !empty($name) ? $name : '',
              'ym_pl_type' => !empty($polyline['ym_pl_type']) ? $polyline['ym_pl_type'] : UNIMAPS_POLYLINE_TYPE,
              'ym_pl_color' => !empty($polyline['ym_pl_color']) ? $polyline['ym_pl_color'] : UNIMAPS_POLYLINE_COLOR,
              'ym_pl_width' => !empty($polyline['ym_pl_width']) ? $polyline['ym_pl_width'] : UNIMAPS_POLYLINE_WIDTH,
              'ym_pl_opacity' => !empty($polyline['ym_pl_opacity']) ? $polyline['ym_pl_opacity'] : UNIMAPS_POLYLINE_OPACITY,
              'ym_pl_drag' => $data['polyline']['ym_pl_drag'],
              'ym_pl_edit' => 0,
            );
            if (user_access('view balloon content')) $params[$style][0]['ym_pl_descr'] = (is_array($descr) && isset($descr['value'])) ? $descr['value'] : $descr;
            $params['points_id'] = '#edit-points-' . $delta . ' input';
            $element += unimaps_yandex_polyline_form($params[$style][0], $delta, $points);
          }
          // выводить только для полигонов
          elseif (module_exists('unimaps_yandex_shapes') && user_access('view placemark') && $entity->type == 'ym_polygon') {
            $style = 'polygones';
            $params[$style][0] = array(
              'points' => isset($points) ? $points : array(),
              'ym_pg_name' => !empty($name) ? $name : '',
              'ym_pg_style' => !empty($polygon['ym_pg_style']) ? $polygon['ym_pg_style'] : UNIMAPS_POLYGON_STYLE,
              'ym_pg_width' => !empty($polygon['ym_pg_width']) ? $polygon['ym_pg_width'] : UNIMAPS_POLYGON_WIDTH,
              'ym_pg_color' => !empty($polygon['ym_pg_color']) ? $polygon['ym_pg_color'] : UNIMAPS_POLYGON_COLOR,
              'ym_pg_fill' => isset($polygon['ym_pg_fill']) ? (integer)$polygon['ym_pg_fill'] : UNIMAPS_POLYGON_FILL,
              'ym_pg_fillcolor' => !empty($polygon['ym_pg_fillcolor']) ? $polygon['ym_pg_fillcolor'] : UNIMAPS_POLYGON_FILLCOLOR,
              'ym_pg_opacity' => !empty($polygon['ym_pg_opacity']) ? $polygon['ym_pg_opacity'] : UNIMAPS_POLYGON_OPACITY,
              'ym_pg_drag' => $data['polygon']['ym_pg_drag'],
              'ym_pg_edit' => 0,
            );
            if (user_access('view balloon content')) $params[$style][0]['ym_pg_descr'] = (is_array($descr) && isset($descr['value'])) ? $descr['value'] : $descr;
            $params['points_id'] = '#edit-points-' . $delta . ' input';
            $element += unimaps_yandex_polygon_form($params[$style][0], $delta, $points);
          }

          // выводить только для меток
          if (!isset($style)) $style = 'placemarks';
          if ($style == 'placemarks' && user_access('view placemark')) {
            if ($lat != UNIMAPS_LATITUDE && $lng != UNIMAPS_LONGITUDE) {
              $params[$style] = array(
                0 => array(
                  'title' => $name,
                  'lat' => $lat,
                  'lng' => $lng,
                  'yandex_icon' => $yandex_icon,
                  'drag' => 0,
                ),
              );
              if (user_access('view balloon content')) $params[$style][0]['descr'] = (is_array($descr) && isset($descr['value'])) ? $descr['value'] : $descr;
            }
          }

          $params['map_style'] = $style;

          if (isset($options['controls']['maptools'])) $params['maptools'] = 1;
          if (isset($options['controls']['typecontrol'])) $params['typecontrol'] = 1;
          if (isset($options['controls']['minimap'])) $params['minimap'] = 1;
          if (isset($options['controls']['scaleline'])) $params['scaleline'] = 1;
          if (isset($options['controls']['search'])) $params['search'] = 1;
          if (isset($options['controls']['traffic'])) $params['traffic'] = 1;

          if (isset($options['behaviors']['drag'])) $params['drag'] = 1;
          if (isset($options['behaviors']['scrollzoom'])) $params['scrollzoom'] = 1;
          if (isset($options['behaviors']['dblclickzoom'])) $params['dblclickzoom'] = 1;
          if (isset($options['behaviors']['multitouch'])) $params['multitouch'] = 1;

          $element[$delta]['#markup'] = '<div id="YMapsID-' . $map_id . '" style="width: ' . $width . 'px; height: ' . $height . 'px"><ul id="YMapsID-links-' . $map_id . '"></ul>';


           // вызов hook_unimapsapi() с $op = ''
           $object->options = $params;
           unimaps_invoke_unimapsapi($object, 'map', 'node');
           $params = $object->options;

           _unimaps_show_map($map_id, $params);

      }
      break;
  }

  return $element;
}

/**
 * Implements hook_field_widget_info().
 */
function unimaps_field_widget_info() {

  return array(
    'unimaps_yandex' => array(
      'label' => t('Yandex map'),
      'field types' => array('unimaps_latlng'),
    ),
  );
}

function unimaps_form_field_ui_field_edit_form_alter(&$form, &$form_state, $form_id) {

  $form['#submit'][] = 'unimaps_field_widget_form_submit';
}

function unimaps_field_widget_form_submit($form, &$form_state) {

  $field_name = unimaps_get_fields($form_state['complete form']['#instance']['bundle']);

  if ($form_state['complete form']['#instance']['field_name'] == $field_name) {

    $descr = serialize($form_state['values'][$field_name][LANGUAGE_NONE][0]['descr']);
    $form_state['values'][$field_name][LANGUAGE_NONE][0]['descr'] = $descr;

    $options = $form_state['values'][$field_name][LANGUAGE_NONE][0]['options'];

    if ($options) {
      foreach ($options['controls'] as $k => $v) {
        if ($v == '0') unset($form_state['values'][$field_name][LANGUAGE_NONE][0]['options']['controls'][$k]);
      }
      foreach ($options['behaviors'] as $k => $v) {
        if ($v == '0') unset($form_state['values'][$field_name][LANGUAGE_NONE][0]['options']['behaviors'][$k]);
      }
    }

  }
}

/**
 * Implements hook_field_widget_form().
 */
function unimaps_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {

    if (user_access('insert map in node edit form')) {

      if (arg(0) == 'node' && (is_numeric(arg(1)) || arg(1) == 'add')) {
        $map_id = isset($items[$delta]['map_id']) ? 'unimaps-' . $items[$delta]['map_id'] . '-' . $delta  : 'unimaps-node-' . (is_numeric(arg(1)) ? $element['#entity']->nid : 'add-' . $element['#entity']->type) . '-' . $delta;
        $field_name = unimaps_get_fields($element['#entity']->type);
        $dl = is_numeric(arg(1)) ? $delta : 0;
      }
      else {
        $map_id = 'unimaps-' . $element['#bundle'] . '-' . $element['#field_name'] . '-' . $delta;
        $field_name = $element['#field_name'];
        $dl = 0;
      }

      $name = isset($items[$dl]['name']) ? $items[$dl]['name'] : '';
      $descr = isset($items[$dl]['descr']) ? $items[$dl]['descr'] : array();
      $lat = isset($items[$dl]['lat']) ? $items[$dl]['lat'] : UNIMAPS_LATITUDE;
      $lng = isset($items[$dl]['lng']) ? $items[$dl]['lng'] : UNIMAPS_LONGITUDE;
      $yandex_icon = isset($items[$dl]['yandex_icon']) ? $items[$dl]['yandex_icon'] : (variable_get('unimaps_yandex_api', 2) ? UNIMAPS_YANDEX2_ICON : UNIMAPS_YANDEX_ICON);

      if (arg(0) == 'node' && is_numeric(arg(1))) {
        $data = isset($items[$dl]['data']) ? unserialize($items[$dl]['data']) : array();

        $options = (isset($data['options'])) ? $data['options'] : unimaps_default_options();

        $zoom = isset($data['zoom']) ? $data['zoom'] : UNIMAPS_YMAP_ZOOM;
        $type = isset($data['type']) ? $data['type'] : UNIMAPS_YMAP_TYPE;

        if (module_exists('unimaps_yandex_shapes') && isset($data['polyline'])) {
          $polyline = $data['polyline'];
          if (isset($polyline['points'])) $points = unimaps_coords_decode($polyline['points']);
        }

        if (module_exists('unimaps_yandex_shapes') && isset($data['polygon'])) {
          $polygon = $data['polygon'];
          if (isset($polygon['points'])) $points = unimaps_coords_decode($polygon['points']);
        }
      if (module_exists('unimaps_yandex_shapes') && isset($data['polygon'])) {
        $polygon = $data['polygon'];
      }
    }
    else {

      $options = isset($items[$dl]['options']) ? $items[$dl]['options'] : unimaps_default_options();

      $zoom = isset($items[$dl]['zoom']) ? $items[$dl]['zoom'] : UNIMAPS_YMAP_ZOOM;
      $type = isset($items[$dl]['type']) ? $items[$dl]['type'] : UNIMAPS_YMAP_TYPE;

      if (module_exists('unimaps_yandex_shapes') && isset($items[$dl]['polyline'])) $polyline = $items[$dl]['polyline'];
      if (module_exists('unimaps_yandex_shapes') && isset($items[$dl]['polygon'])) $polygon = $items[$dl]['polygon'];
    }

    if (isset($options)) {
      foreach ($options['controls'] as $k => $v) {
        if ($v == '0') unset($options['controls'][$k]);
      }
      foreach ($options['behaviors'] as $k => $v) {
        if ($v == '0') unset($options['behaviors'][$k]);
      }
    }

    $zoom_type = isset($options['zoom_type']) ? $options['zoom_type'] : UNIMAPS_YMAP_ZOOMTYPE;

    $height = isset($options['height']) ? $options['height'] : UNIMAPS_MAP_HEIGHT;
    $width = isset($options['width']) ? $options['width'] : UNIMAPS_MAP_WIDTH;

    $element += array(
      '#delta' => $delta,
    );

    switch ($instance['widget']['type']) {

      case 'unimaps_yandex':

          // выводить форму редактирования только при наличии у юзера соотв. прав
          $element['map'] = array(
            '#type' => 'item',
            '#markup' => '<div id="YMapsID-' . $map_id . '" style="width: ' . $width . 'px; height: ' . $height . 'px">',
            '#description' => t('Drag map by holding left button. Create marker by click.'),
            '#element_validate' => array('unimaps_map_check'),
          );

          if (user_access('remove placemark')) {
            $element['delete'] = array(
              '#type' => 'button',
              '#value' => t('Delete'),
              '#prefix' => '<div id="edit-delete-' . $delta . '">',
              '#suffix' => '</div>',
              '#attributes' => array('class' => array('unimaps-delete')),
            );
          }

          // НЕ выводить для линий и полигонов
          if (!in_array($element['#bundle'], array('ym_polyline', 'ym_polygon'))) {
            $element['yandex_icon'] = array(
              '#title' => t('Yandex icon'),
              '#type' => 'textfield',
              '#default_value' => $yandex_icon,
              '#size' => 30,
              '#maxlength' => 30,
              '#description' => t('Yandex icon for the placemark'),
              '#attributes' => array('class' => array('unimaps-icon')),
              '#prefix' => '<div id="edit-icon-' . $delta . '">',
              '#suffix' => '</div>',
              '#disabled' => !user_access('select placemark icon'),
            );
          }

          $element['name'] = array(
            '#title' => t('Name'),
            '#type' => 'textfield',
            '#default_value' => $name,
            '#description' => t('Name of the placemark'),
          );

          $element['descr'] = array(
            '#title' => t('Description'),
            '#type' => 'text_format',
            '#default_value' => (is_array($descr) && isset($descr['value'])) ? $descr['value'] : $descr,
            '#description' => t('Description of the placemark'),
            '#format' => filter_default_format(),
          );

          $element['lat'] = array(
            '#type' => 'hidden',
            '#default_value' => $lat,
            '#prefix' => '<div id="edit-lat-' . $delta . '">',
            '#suffix' => '</div>',
          );

          $element['lng'] = array(
            '#type' => 'hidden',
            '#default_value' => $lng,
            '#prefix' => '<div id="edit-lng-' . $delta . '">',
            '#suffix' => '</div>',
          );

          $element['zoom'] = array(
            '#type' => 'hidden',
            '#default_value' => $zoom,
            '#prefix' => '<div id="edit-zoom-' . $delta . '">',
            '#suffix' => '</div>',
          );

          $element['type'] = array(
            '#type' => 'hidden',
            '#default_value' => $type,
            '#prefix' => '<div id="edit-type-' . $delta . '">',
            '#suffix' => '</div>',
          );

          $element['options'] = array(
            '#type' => 'fieldset',
            '#title' => t('Map options'),
            '#collapsible' => TRUE,
            '#collapsed' => TRUE,
          );

          $element['options']['height'] = array(
            '#title' => t('Map height'),
            '#type' => 'textfield',
            '#default_value' => $height,
            '#size' => 4,
            '#maxlength' => 4,
            '#description' => t('Yandex map height'),
          );

          $element['options']['width'] = array(
            '#title' => t('Map width'),
            '#type' => 'textfield',
            '#default_value' => $width,
            '#size' => 4,
            '#maxlength' => 4,
            '#description' => t('Yandex map width'),
          );

          // пример: http://api.drupal.org/api/drupal/modules!comment!comment.module/function/comment_form_node_form_alter/7
          // тип элемента управления масштабом
          $element['options']['zoom_type'] = array(
            '#type' => 'select',
            '#title' => t('Type of zoom control'),
            '#description' => t('Select zoom control type'),
            '#default_value' => $zoom_type,
            '#options' => array(
              'NONE' => t('None'),
              'SMALL' => t('Small Zoom Control'),
              'BASE' => t('Zoom Control'),
            ),
          );

          $element['options']['controls'] = array(
            '#title' => t('Map controls'),
            '#type' => 'checkboxes',
            '#options' => array(
              'maptools' => t('Map tools'),
              'minimap' => t('Mini map'),
              'typecontrol' => t('Type control'),
              'scaleline' => t('Scaleline'),
              'traffic' => t('Traffic control'),
              'search' => t('Search'),
            ),
            '#default_value' => $options['controls'],
          );

          $element['options']['behaviors'] = array(
            '#title' => t('Map behaviors'),
            '#type' => 'checkboxes',
            '#options' => array(
              'drag' => t('Enable map dragging'),
              'scrollzoom' => t('Enable scrool zoom'),
              'dblclickzoom' => t('Enable double click zoom'),
              'multitouch' => t('Multitouch support'),
            ),
            '#default_value' => $options['behaviors'],
          );

          $params = array(
            'map_id' => $map_id,
            'map_delta' => $delta,
            'latitude' => $lat,
            'longitude' => $lng,
            'map_height' => $height,
            'map_width' => $width,
            'map_type' => $type,
            'map_zoom' => $zoom,
            'zoom' => ($zoom_type == 'NONE') ? '' : $zoom_type,
            'add' => TRUE,
            'add_icon' => $yandex_icon,
            'add_title' => $name,
            'add_descr' => isset($descr['value']) ? $descr['value'] : $descr,
            'add_cust_icon' => '',
            'can_add' => UNIMAPS_CAN_ADD_PLACEMARKS,
            'lat_id' => '#edit-lat-' . $dl . ' input',
            'lng_id' => '#edit-lng-' . $dl . ' input',
            'zoom_id' => '#edit-zoom-' . $dl . ' input',
            'type_id' => '#edit-type-' . $dl . ' input',
            'icon_id' => '#edit-icon-' . $dl . ' input',
            'addr_id' => '#edit-addr-' . $dl . ' input',
            'delete_id' => '#edit-delete-' . $dl . ' input',
          );

          // выводить только для линий
          if (module_exists('unimaps_yandex_shapes') && user_access('view placemark') && $element['#bundle'] == 'ym_polyline') {
            $style = 'lines';
            $params[$style][0] = array(
              'ym_pl_name' => !empty($name) ? $name : '',
              'ym_pl_type' =>  !empty($polyline['ym_pl_type']) ? $polyline['ym_pl_type'] : UNIMAPS_POLYLINE_TYPE,
              'ym_pl_color' => !empty($polyline['ym_pl_color']) ? $polyline['ym_pl_color'] : UNIMAPS_POLYLINE_COLOR,
              'ym_pl_width' => !empty($polyline['ym_pl_width']) ? $polyline['ym_pl_width'] : UNIMAPS_POLYLINE_WIDTH,
              'ym_pl_opacity' => !empty($polyline['ym_pl_opacity']) ? $polyline['ym_pl_opacity'] : UNIMAPS_POLYLINE_OPACITY,
              'ym_pl_drag' => 1,
              'ym_pl_edit' => 1,
            );
            if (isset($points)) $params['lines'][0]['points'] = $points;
            if (user_access('view balloon content')) $params[$style][0]['ym_pl_descr'] = (is_array($descr) && isset($descr['value'])) ? $descr['value'] : $descr;
            $params['points_id'] = '#edit-points-' . $dl . ' input';
            $element += unimaps_yandex_polyline_form($params[$style][0], $dl, $polyline['points']);
          }

          // выводить только для полигонов
          elseif (module_exists('unimaps_yandex_shapes') && user_access('view placemark') && $element['#bundle'] == 'ym_polygon') {
            $style = 'polygones';
            $params[$style][0] = array(
              'ym_pg_name' => !empty($name) ? $name : '',
              'ym_pg_style' => !empty($polygon['ym_pg_style']) ? $polygon['ym_pg_style'] : UNIMAPS_POLYGON_STYLE,
              'ym_pg_width' => !empty($polygon['ym_pg_width']) ? $polygon['ym_pg_width'] : UNIMAPS_POLYGON_WIDTH,
              'ym_pg_color' => !empty($polygon['ym_pg_color']) ? $polygon['ym_pg_color'] : UNIMAPS_POLYGON_COLOR,
              'ym_pg_fill' => !empty($polygon['ym_pg_fill']) ? (integer)$polygon['ym_pg_fill'] : UNIMAPS_POLYGON_FILL,
              'ym_pg_fillcolor' => !empty($polygon['ym_pg_fillcolor']) ? $polygon['ym_pg_fillcolor'] : UNIMAPS_POLYGON_FILLCOLOR,
              'ym_pg_opacity' => !empty($polygon['ym_pg_opacity']) ? $polygon['ym_pg_opacity'] : UNIMAPS_POLYGON_OPACITY,
              'ym_pg_drag' => 1,
              'ym_pg_edit' => 1,
            );
            if (isset($points)) $params['polygones'][0]['points'] = $points;
            if (user_access('view balloon content')) $params[$style][0]['ym_pg_descr'] = (is_array($descr) && isset($descr['value'])) ? $descr['value'] : $descr;
            $params['points_id'] = '#edit-points-' . $dl . ' input';
            $element += unimaps_yandex_polygon_form($params[$style][0], $dl, $polygon['points']);
          }

          // выводить только для меток
          if (!isset($style)) $style = 'placemarks';
          if ($style == 'placemarks' && user_access('view placemark')) {
            if ($lat != UNIMAPS_LATITUDE && $lng != UNIMAPS_LONGITUDE) {
              $params[$style] = array(
                0 => array(
                  'lat' => $lat,
                  'lng' => $lng,
                  'title' => $name,
                  'yandex_icon' => $yandex_icon,
                  'drag' => TRUE,
                ),
              );
              if (user_access('view balloon content')) $params[$style][0]['descr'] = (is_array($descr) && isset($descr['value'])) ? $descr['value'] : $descr;
            }
          }

         $params['map_style'] = $style;

          if (isset($options['controls']['maptools'])) $params['maptools'] = 1;
          if (isset($options['controls']['typecontrol'])) $params['typecontrol'] = 1;
          if (isset($options['controls']['minimap'])) $params['minimap'] = 1;
          if (isset($options['controls']['scaleline'])) $params['scaleline'] = 1;
          if (isset($options['controls']['search'])) $params['search'] = 1;
          if (isset($options['controls']['traffic'])) $params['traffic'] = 1;

          if (isset($options['behaviors']['drag'])) $params['drag'] = 1;
          if (isset($options['behaviors']['scrollzoom'])) $params['scrollzoom'] = 1;
          if (isset($options['behaviors']['dblclickzoom'])) $params['dblclickzoom'] = 1;
          if (isset($options['behaviors']['multitouch'])) $params['multitouch'] = 1;

           // вызов hook_unimapsapi() с $op = ''
           $object->options = $params;
           unimaps_invoke_unimapsapi($object, 'map', 'node');
           $params = $object->options;


          // вывести карту
          _unimaps_show_map($map_id, $params);

          break;
      }
     }

  return $element;
}


/**
 * Implements hook_field_presave().
 */
function unimaps_field_presave($entity_type, $entity, $field, $instance, $langcode, &$items) {

  foreach ($items AS $delta => $item) {

    // "Упаковка" всех дополнительных параметров метки в поле 'data'
    $data = array();
    foreach ($item as $f => $name) {
      if (!in_array($f, array('data', 'lat', 'lng', 'name', 'descr', 'yandex_icon'))) {
        $data[$f] = $name;
      }
    }
    $item['data'] = serialize($data);

    if (is_array($item['descr'])) {
      $descr = $item['descr']['value'];
      $item['descr'] = $descr;
    }

    $items[$delta] = $item;
  }

}

/**
 * Implements hook_field_widget_error().
 */
function unimaps_field_widget_error($element, $error, $form, &$form_state) {

  form_error($element, $error['message']);
}

// Определение названий полей, используемых в текущем материале
function unimaps_get_fields($bundle = NULL, $field = TRUE) {

  // Поиск среди всех полей тех, которые использует unimaps в ноде заданного типа
  if ($field) foreach (field_info_fields() as $id => $field) {
    if ($field['module'] == 'unimaps' && in_array($bundle, $field['bundles']['node'])) {
      return $field['field_name'];
    }
  }
}

// Формирование значений по умолчанию для списка чекбоксов в параметрах карты
function unimaps_default_options() {

  return array(
    'controls' => array('maptools' => 'maptools', 'typecontrol' => 'typecontrol'),
    'behaviors' => array('drag' => 'drag', 'scrollzoom' => 'scrollzoom', 'dblclickzoom' => 'dblclickzoom', 'multitouch' => 'multitouch'),
  );
}

// Декодирование строки в массив координат вершин (только для линий и полигонов)
function unimaps_coords_decode($coords = '') {

  $points = array();
  if (!empty($coords)) preg_match_all('/\[\s*(\d+.?\d+)\s*?,\s*?(\d+.?\d+)\s*\]/s', $coords, $matches);
  if (isset($matches)) foreach($matches[0] as $k => $v) {
    $points[$k]['lat'] = $matches[1][$k];
    $points[$k]['lng'] = $matches[2][$k];
  }

  return $points;
}