<?php

class unimaps_views_plugin_style_ymap extends views_plugin_style {

  function options(&$options) {

    parent::options($options);

    return $options;
  }

  function options_form(&$form, &$form_state) {

    parent::options_form($form, $form_state);


    $handlers = $this->display->handler->get_handlers('field');
    if (empty($handlers)) {
      $form['error_markup'] = array(
       '#markup' => '<div class="error form-item description">' . t('You need at least one field before you can configure your Yandex Maps settings') . '</div>',
      );
      return;
    }

    // ширина карты
    $form['map_width'] = array(
      '#type' => 'textfield',
      '#title' => t('Map width'),
      '#description' => t('Width of map'),
      '#default_value' => !empty($this->options['map_width']) ? $this->options['map_width'] : UNIMAPS_MAP_WIDTH,
      '#size' => 3,
    );
    // высота карты
    $form['map_height'] = array(
      '#type' => 'textfield',
      '#title' => t('Map height'),
      '#description' => t('Height of map'),
      '#default_value' => !empty($this->options['map_height']) ? $this->options['map_height'] : UNIMAPS_MAP_HEIGHT,
      '#size' => 3,
    );
    // тип карты по умолчанию
    $form['map_type'] = array(
      '#type' => 'select',
      '#title' => t('Map type'),
      '#description' => t('Type of map'),
      '#default_value' => !empty($this->options['map_type']) ? $this->options['map_type'] : UNIMAPS_YMAP_TYPE,
      '#options' => array(
        'MAP' => t('Map'),
        'SATELLITE' => t('Satellite'),
        'HYBRID' => t('Hybrid'),
        'PMAP' => t('Public map'),
        'PHYBRID' => t('Public hybrid'),
      ),
    );
    // широта центра карты
    $form['latitude'] = array(
      '#type' => 'textfield',
      '#title' => t('Center latitude'),
      '#description' => t('Latitude of map center'),
      '#default_value' => !empty($this->options['latitude']) ? $this->options['latitude'] : UNIMAPS_LATITUDE,
      '#size' => 7,
    );
    // долгота центра карты
    $form['longitude'] = array(
      '#type' => 'textfield',
      '#title' => t('Center longitude'),
      '#description' => t('Longitude of map center'),
      '#default_value' => !empty($this->options['longitude']) ? $this->options['longitude'] : UNIMAPS_LONGITUDE,
      '#size' => 7,
    );
    // центрирование карты по IP посетителя
    if (module_exists('unimaps_ipgeobase')) {
      $form['ip_center'] = array(
        '#type' => 'checkbox',
        '#title' => t('Use IPGeobase for map centering'),
        '#description' => t('See ') . l('IPGeobase module', 'http://drupal.org/project/ipgeobase', array('attributes' => array('target' => '_blank'))) . ' description',
        '#default_value' => $this->options['ip_center'],
      );
    }
    // масштаб карты
    $form['map_zoom'] = array(
      '#type' => 'select',
      '#title' => t('Map zoom level'),
      '#description' => t('Map zoom level'),
      '#options' => array(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18),
      '#default_value' => !empty($this->options['map_zoom']) ? $this->options['map_zoom'] : UNIMAPS_YMAP_ZOOM,
    );
    // тип элемента управления масштабом
    $form['zoom'] = array(
      '#type' => 'select',
      '#title' => t('Type of zoom control'),
      '#description' => t('Select zoom control type'),
      '#default_value' => !empty($this->options['zoom']) ? $this->options['zoom'] : UNIMAPS_YMAP_ZOOMTYPE,
      '#options' => array(
        '' => t('None'),
        'SMALL' => t('Small Zoom Control'),
        'BASE' => t('Zoom Control'),
      ),
    );
    if (module_exists('unimaps_geocoder')) $form['search']['#options']['form'] = t('Search form');

    $form['options'] = array(
      '#type' => 'fieldset',
      '#title' => t('Map options'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );

    // пример: http://api.drupal.org/api/drupal/modules!comment!comment.module/function/comment_form_node_form_alter/7
    $form['options']['controls'] = array(
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
      '#default_value' => !empty($this->options['options']['controls']) ? $this->options['options']['controls'] : array('maptools', 'typecontrol', 'scaleline'),
    );

    $form['options']['objects'] = array(
      '#title' => t('Objects on map'),
      '#type' => 'checkboxes',
      '#options' => array(
          'list' => t('Objects list'),
          'numbers' => t('Numbers in list'),
      ),
      '#default_value' => !empty($this->options['options']['objects']) ? $this->options['options']['objects'] : array(),
    );

    $form['options']['behaviors'] = array(
      '#title' => t('Map behaviors'),
      '#type' => 'checkboxes',
      '#options' => array(
          'drag' => t('Enable map dragging'),
          'scrollzoom' => t('Enable scrool zoom'),
          'dblclickzoom' => t('Enable double click zoom'),
          'multitouch' => t('Multitouch support'),
      ),
      '#default_value' => !empty($this->options['options']['behaviors']) ? $this->options['options']['behaviors'] : array('drag', 'scrollzoom', 'dblclickzoom', 'multitouch'),
    );

/*
    // стиль по умолчанию
    $form['plugin_style'] = array(
      '#type' => 'select',
      '#title' => t('Plugin style'),
      '#description' => t('Style of plugin'),
      '#default_value' => !empty($this->options['plugin_style']) ? $this->options['plugin_style'] : UNIMAPS_PLUGIN_STYLE,
      '#options' => array(
        'default' => t('Default'),
      ),
      '#disabled' => TRUE,
    );
    if (module_exists('unimaps_yandex_shapes')) {
      $form['plugin_style']['#disabled'] = FALSE;
      $form['plugin_style']['#options']['lines'] = t('Polylines');
      $form['plugin_style']['#options']['polygones'] = t('Polygones');

      $form += unimaps_yandex_polyline_form($this->options['polyline']);
      $form += unimaps_yandex_polygon_form($this->options['polygon']);

    }
    if (module_exists('unimaps_yandex_routes')) {
      $form['plugin_type']['#options']['routes'] = t('Routes');
      $form['plugin_type']['#disabled'] = FALSE;
    }
*/

  }

  function render() {
    global $user;

    parent::options_form($form, $form_state);

    // стиль плагина
    $plugin_style = !empty($this->options['plugin_style']) ? $this->options['plugin_style'] : UNIMAPS_PLUGIN_STYLE;

    $display = $this->view->current_display;
    $handlers = $this->view->display[$display]->handler->handlers['field'];

    if ($this->options['options']['controls']) foreach($this->options['options']['controls'] as $k => $v) {
      if (!empty($v)) $this->options[$k] = 1;
    }

    if ($this->options['options']['objects']) foreach($this->options['options']['objects'] as $k => $v) {
      if (!empty($v)) $this->options[$k] = 1;
    }

    if ($this->options['options']['behaviors']) foreach($this->options['options']['behaviors'] as $k => $v) {
      if (!empty($v)) $this->options[$k] = 1;
    }
    unset($this->options['options']);
    unset($this->options['grouping']);
    unset($this->options['polyline']);
    unset($this->options['polygon']);

    $this->options['map_delta'] = 0;

    // карта поддерживает JS
    $this->options['map_js'] = TRUE;

    // источник данных для карты (views/node)
    $this->options['src'] = 'views';

    // идентификатор карты
    $map_id = 'unimaps-views-' . $this->view->name . '-' . $display;
    $this->options['map_id'] = $map_id;

    // список меток
    if ($plugin_style == 'default') {
      if (user_access('view placemark')) $this->options += unimaps_views_get_objects($this->view->result, $handlers);
    }

    if (module_exists('unimaps_click')) {
      $this->options['can_add'] = UNIMAPS_CAN_ADD_PLACEMARKS;
      $this->options['add'] = user_access('click form') && user_access('add placemark');
      $this->options['click'] = user_access('click form');
    }
    else {
      $this->options['can_add'] = 0;
      $this->options['add'] = $this->options['click'] = FALSE;
    }

    // вызов hook_unimapsapi() с $op = 'map'
    unimaps_invoke_unimapsapi($this, 'map');

    if (!empty($this->view->display[$display]->display_options['path'])) $this->options['path'] = url($this->view->display[$display]->display_options['path']);

    _unimaps_show_map($map_id, $this->options);

    $this->options['map_name'] = $this->view->current_display;

    return theme($this->theme_functions(), array(
      'view' => $this->view,
      'options' => $this->options,
      'rows' => '',
    ));

  }

}