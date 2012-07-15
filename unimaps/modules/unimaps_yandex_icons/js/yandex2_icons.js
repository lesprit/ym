(function ($) {

  $(document).ready(function () {

    // Параметры карты и меток, переданные из модуля
    var options = Drupal.settings.yandex_maps;

    var imageSrcBase = "http://api.yandex.ru/maps/doc/jsapi/2.x/ref/images/styles/";

    // массив с параметрами значков
    var pack = new Array(
        ['twirl#blueIcon',	'http://api.yandex.ru/maps/doc/jsapi/2.x/ref/images/styles/blue.png'],
        ['twirl#redIcon',	'http://api.yandex.ru/maps/doc/jsapi/2.x/ref/images/styles/red.png'],
        ['twirl#greenIcon',	'http://api.yandex.ru/maps/doc/jsapi/2.x/ref/images/styles/green.png'],
	['twirl#yellowIcon',	'http://api.yandex.ru/maps/doc/jsapi/2.x/ref/images/styles/yellow.png'],
	['twirl#violetIcon',	'http://api.yandex.ru/maps/doc/jsapi/2.x/ref/images/styles/violet.png'],
	['twirl#orangeIcon',	'http://api.yandex.ru/maps/doc/jsapi/2.x/ref/images/styles/orange.png'],
	['twirl#darkblueIcon',	'http://api.yandex.ru/maps/doc/jsapi/2.x/ref/images/styles/darkblue.png'],
	['twirl#pinkIcon',	'http://api.yandex.ru/maps/doc/jsapi/2.x/ref/images/styles/pink.png'],
	['twirl#darkgreenIcon',	'http://api.yandex.ru/maps/doc/jsapi/2.x/ref/images/styles/darkgreen.png'],
	['twirl#darkorangeIcon','http://api.yandex.ru/maps/doc/jsapi/2.x/ref/images/styles/darkorange.png'],
	['twirl#whiteIcon',	'http://api.yandex.ru/maps/doc/jsapi/2.x/ref/images/styles/white.png'],
	['twirl#greyIcon',	'http://api.yandex.ru/maps/doc/jsapi/2.x/ref/images/styles/grey.png'],
	['twirl#lightblueIcon',	'http://api.yandex.ru/maps/doc/jsapi/2.x/ref/images/styles/lightblue.png'],
	['twirl#brownIcon',	'http://api.yandex.ru/maps/doc/jsapi/2.x/ref/images/styles/brown.png'],
	['twirl#nightIcon',	'http://api.yandex.ru/maps/doc/jsapi/2.x/ref/images/styles/night.png'],
	['twirl#blackIcon',	'http://api.yandex.ru/maps/doc/jsapi/2.x/ref/images/styles/black.png']
      );

    // формирование массива опций в формате ddslick
    var ddData = new Array();
    $.each(pack, function(id, icon) {

      ddData.push({
        value: icon[0],
        description: icon[0],
        imageSrc: icon[1]
      });
    });

    // для всех текстовых полей ввода значков в форме
    $('input.unimaps-icon').each(function() {

      var i = 0;
      // название значка
      var icon = $(this).val();
      var v = -1;

//console.log($(this).val());

      // для всех элементов массива
      $.each(ddData, function(id, d) {
        // значение элемента массива совпадает с названием значка ?
        if (icon == d.value) {
          // если да - сохранить его номер
          v = i;
          return;
        }
        i++;
      });

      // инициализировать выпадающие спискм для всех текстовых полей
      // ввода значков
      $(this).ddslick({
        data: ddData,
        width: 180,
        // значение по умолчанию для выпадающего списка
        defaultSelectedIndex: v,
        // при выборе из списка элемента
        onSelected: function(data) {
          // добавить его в поле ввода значка
          var f = $('#' + data.original.context.id + ' input.dd-selected-value');
          f.attr('name', data.original.context.name);
          f.trigger('change');
        }
      });

    });

  });

})(jQuery);