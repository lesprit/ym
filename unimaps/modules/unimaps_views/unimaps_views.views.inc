<?php

function unimaps_views_views_plugins() {

  return array(
    'module' => 'unimaps_views',
    'style' => array(
      'ymap' => array(
        'title' => t('Yandex Map'),
        'help' => t('Displays rows as a Yandex map.'),
        'handler' => 'unimaps_views_plugin_style_ymap',
        'theme' => 'unimaps-view-ymap',
        'uses row plugin' => TRUE,
        'uses grouping' => FALSE,
        'uses options' => TRUE,
        'type' => 'normal',
        'help topic' => 'style-ymap',
      ),
    ),
  );
}
