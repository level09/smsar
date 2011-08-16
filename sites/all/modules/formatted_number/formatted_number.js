// $Id: formatted_number.js,v 1.1.2.4 2009/09/20 11:35:40 markuspetrux Exp $

(function ($) {

/**
 * Formatted number behavior.
 *
 * Formatted number fields are processed during the following events:
 * - on page load     : All number fields are formatted with site/user defined options.
 * - on element focus : Thousands separator are removed, if any.
 * - on element blur  : Number is formatted again with site/user defined options.
 * - on form submit   : Thousands separator are removed to prevent from triggering
 *                      maxlength error during FAPI validation.
 */
Drupal.behaviors.formatted_number = function(context) {
  $('input.formatted-number:not(.formatted-number-processed)', context).each(function() {
    // Make sure this element is not processed more than once.
    $(this).addClass('formatted-number-processed');

    // Number of decimal places for this element.
    var decimals = $(this).attr('decimals');
    decimals = (decimals == undefined ? -1 : Math.max(-1, parseInt(decimals)));

    // The element is properly formatted on page load.
    Drupal.formatted_number.formatElement(this, decimals);

    // Bind element events.
    $(this).bind('focus', function() {
      Drupal.formatted_number.clearThousandsSep(this);
    }).bind('blur', function() {
      Drupal.formatted_number.formatElement(this, decimals);
    });

    $(this).parents('form:not(.formatted-number-processed)').each(function() {
      // Make sure this form is not processed more than once.
      $(this).addClass('formatted-number-processed');

      // Clear thousands separators before submitting. This is not strictly necessary
      // because input is always validated on the server, but it prevents from getting
      // "field cannot be longer than max" errors issued by FAPI.
      $(this).bind('submit', function() {
        $('input.formatted-number', this).each(function() {
          Drupal.formatted_number.clearThousandsSep(this);
        });
      });
    });
  });
};

/**
 * Provide our own namespace for function names.
 */
Drupal.formatted_number = {};

/**
 * Clear thousands separators from the given input element.
 *
 * @param element
 *   The input element.
 */
Drupal.formatted_number.clearThousandsSep = function(element) {
  var number = $(element).val();
  if (number.length > 0 && Drupal.settings.format_number.thousands_sep.length > 0) {
    var thsep = Drupal.settings.format_number.thousands_sep;
    if (thsep == '\u00A0') {
      thsep += ' ';
    }
    number = number.replace(new RegExp('['+ thsep +']', 'g'), '');
    $(element).val(number);
  }
};

/**
 * Format the number in the given element with site/user defined options.
 *
 * @param element
 *   The input element.
 * @param decimals
 *   Number of decimal digits.
 */
Drupal.formatted_number.formatElement = function(element, decimals) {
  var number = $(element).val();
  if (number.length > 0) {
    number = Drupal.parseNumber(number, false);
    if (typeof(number) == 'number') {
      number = Drupal.formatNumber(number, decimals);
    }
    $(element).val(number);
  }
};

})(jQuery);
