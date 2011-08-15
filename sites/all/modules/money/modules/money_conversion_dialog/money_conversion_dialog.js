// $Id: money_conversion_dialog.js,v 1.1.2.2 2008/11/23 18:32:20 markuspetrux Exp $

Drupal.behaviors.moneyConversionDialog = function (context) {
  var acdb;
  $('.money-item:not(.money-item-processed)', context).each(function () {
    if ($('#money-conversion-dialog').length <= 0) {
      $(Drupal.theme('moneyConversionDialog')).appendTo('body');
      if (acdb == undefined) {
        acdb = new Drupal.ACDB(Drupal.settings.moneyConversionDialog.uri);
        acdb.owner = new Drupal.moneyConversionResultHandler();
      }
    }
    Drupal.moneyConversionDialog(this, acdb);
  });
};

Drupal.moneyConversionDialog = function (moneyItem, acdb) {
  // amount, decimals, currency_display_mode, currency.
  var field_settings = $(moneyItem).attr("class").replace(/^.*\[(.*?)\].*$/, "$1");

  $(moneyItem).addClass('money-item-processed').append('&nbsp;').append(
    $(Drupal.theme('moneyConversionIcon')).click(function () {
      if ($('#money-conversion-form').length <= 0) {
        $('#money-conversion-dialog').html(Drupal.theme('moneyConversionForm'));
      }
      $('#money-conversion-select').attr('currency', field_settings.split("|")[3]);
      acdb.search('currency-list');
      $('#money-conversion-dialog').dialog({
        overlay: {opacity: 0.5, background: '#000000'},
        title: Drupal.t('Currency conversion | !amount', {'!amount': $(moneyItem).text()}),
        dialogClass: 'money-conversion-wrapper',
        close: function () { acdb.cancel(); },
        width: '450px', height: '200px',
        resizable: false,
        modal: true
      });
      $('#money-conversion-select').change(function () {
        acdb.search('convert/'+ field_settings + '|' + $(this).val());
      });
    })
  );
};

Drupal.moneyConversionResultHandler = function () {
  this.setStatus = function (status) {
    if (status == 'begin') {
      $('#money-conversion-result').html(Drupal.theme('moneyConversionLoading'));
    }
  };
  this.found = function (result) {
    if (typeof result['error'] == 'string') {
      $('#money-conversion-result').html('');
      $('#money-conversion-error').html(Drupal.checkPlain(result['error']));
    }
    else if (typeof result['money'] == 'string') {
      $('#money-conversion-result').html(Drupal.theme('moneyConversionResult', result['money'], result['from'], result['to']));
      $('#money-conversion-error').html('');
    }
    else if (typeof result['currency-list'] != 'undefined') {
      $('#money-conversion-select').html(Drupal.theme('moneyConversionOptions', result['currency-list']));
      $('#money-conversion-form').show('slow');
      $('#money-conversion-result').html('');
      $('#money-conversion-error').html('');
    }
  };
};

Drupal.theme.prototype.moneyConversionDialog = function () {
  return '<div id="money-conversion-dialog"></div>';
};

Drupal.theme.prototype.moneyConversionForm = function () {
  var output = '<form id="money-conversion-form">';
  output += '<label for="money-conversion-select" id="money-conversion-label">' + Drupal.t('Convert to:') + '</label>';
  output += '<select id="money-conversion-select"></select>';
  output += '</form>';
  output += '<div id="money-conversion-result">' + Drupal.theme('moneyConversionLoading') + '</div>';
  output += '<div id="money-conversion-error"></div>';
  return output;
};

Drupal.theme.prototype.moneyConversionOptions = function (currency_list) {
  var currency_from = $('#money-conversion-select').attr('currency');
  var output = '<option value="">' + Drupal.t('-- Select currency --') + '</option>';
  for (var currency_to in currency_list) {
    if (currency_from != currency_to) {
      output += '<option value="' + currency_to + '">' + Drupal.checkPlain(currency_list[currency_to]) + '</option>';
    }
  }
  return output;
};

Drupal.theme.prototype.moneyConversionIcon = function () {
  return '<img class="money-converter-icon" src="' + Drupal.settings.moneyConversionDialog.path + '/calculator.gif" title="' + Drupal.t('Click to convert!') + '" />';
};

Drupal.theme.prototype.moneyConversionLoading = function () {
  return '<img src="' + Drupal.settings.moneyConversionDialog.path + '/loading.gif" alt="' + Drupal.t('loading ...') + '" /><br />' + Drupal.t('Please, wait...');
};

Drupal.theme.prototype.moneyConversionResult = function (money, from, to) {
  var output = '<div>' + Drupal.checkPlain(money) + '</div>';
  output += '<p><a href="http://finance.yahoo.com/q?s=' + from + to + '=X" target="_blank">' + Drupal.t('Detailed history and chart') + '</a></p>';
  return output;
};
