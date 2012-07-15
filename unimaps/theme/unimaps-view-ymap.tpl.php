<?php $options = $view['view']->style_plugin->options ?>


<?php if (!module_exists('unimaps_blocks')): ?><div id="YMapsID-search"><?php print $options['search_form'] ?></div><?php endif ?>

<div id="YMapsID-form"></div>
<div id="YMapsID<?php print $options['map_id'] ?>" style="width: <?php print $options['map_width'] ? $options['map_width'] : 300 ?>px; height: <?php print $options['map_height'] ? $options['map_height'] : 300 ?>px"></div>
<?php if (!module_exists('unimaps_blocks')): ?><ul id="YMapsID-links"></ul><?php endif ?>
