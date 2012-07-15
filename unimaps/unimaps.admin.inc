<?php

/**
 * @file
 * Administer functions for the unimaps module.
 */

function unimaps_settings_form() {

  $form  = array();

  if (variable_get('unimaps_yandex_api', 2) == 2) {
    $form['unimaps_yandex_key'] = array(
      '#type' => 'textarea',
      '#title' => t('Yandex map key'),
      '#description' => 'http://api.yandex.ru/maps/form.xml',
      '#default_value' => check_plain(variable_get('unimaps_yandex_key', FALSE)),
    );
  }

  $form['unimaps_yandex_api'] = array(
    '#type' => 'select',
    '#title' => t('Yandex API version'),
    '#options' => array(
      1 => 'API 1.x',
      2 => 'API 2.0',
    ),
    '#description' => t('Version of Yandex API'),
    '#default_value' => variable_get('unimaps_yandex_api', 2),
  );

  $yandex_js = _yandex_maps_get_js();

  $form['unimaps_yandex_js'] = array(
    '#type' => 'item',
    '#title' => t('Using yandex.js'),
    '#markup' => !empty($yandex_js) ? $yandex_js : t('Not found'),
  );

  return system_settings_form($form);
}

function unimaps_settings_form_submit() {
}