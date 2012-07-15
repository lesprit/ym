<div id="YMapsID-form"></div>
<div id="YMapsID-<?php print $options['map_id'] ?>" style="width: <?php print $options['map_width'] ? $options['map_width'] : 300 ?>px; height: <?php print $options['map_height'] ? $options['map_height'] : 300 ?>px"></div>
<?php if (!module_exists('unimaps_blocks')): ?><ul id="YMapsID-links-<?php print $options['map_id'] ?>"></ul><?php endif ?>