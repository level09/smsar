// $Id: format_number.js,v 1.1.2.3 2008/11/19 16:53:22 markuspetrux Exp $

/**
 * Format a number with (site default or user defined) thousands separator
 * and decimal point.
 *
 * Formatting options are expected to be at Drupal.settings.format_number.
 *
 * @param float number
 *   The number being formatted.
 * @param int decimals
 *   Number of decimal digits. Use -1 for any number of decimals.
 * @return string
 *   The formatted number.
 */
Drupal.formatNumber = function(number, decimals) {
  if (typeof(number) != "number") {
    number = 0;
  }
  if (typeof(decimals) != "number") {
    decimals = 0;
  }

  // Obtain the sign and separate integer/decimal parts.
  var minus_sign = (number < 0 ? "-" : "");
  var number_parts = (Math.abs(number) + "").split(".");

  // Get the integer part of the number.
  var integer_part = (number_parts[0].length > 0 ? number_parts[0] : "0");

  // Insert thousands separator when necessary.
  if (Drupal.settings.format_number.thousands_sep.length > 0) {
    // Reverse the interger part into an array.
    var digits = integer_part.split("").reverse();
    integer_part = "";
    // Add thousands separator every 3 digits.
    for (var i = 0; i < digits.length; i++) {
      integer_part += ((i % 3) == 0 && i > 0 ? Drupal.settings.format_number.thousands_sep : "") + digits[i];
    }
    // Reverse back the integer part.
    integer_part = integer_part.split("").reverse().join("");
  }
  number = minus_sign + integer_part;

  // Build the decimal part of the number.
  if (decimals > 0) {
    var decimal_part = (number_parts.length <= 1 ? "0" : number_parts[1]);
    if (decimal_part.length > decimals) {
      decimal_part = decimal_part.substr(0, decimals);
    }
    else if (decimal_part.length < decimals) {
      while (decimal_part.length < decimals) { decimal_part += "0"; }
    }
    number += Drupal.settings.format_number.decimal_point + decimal_part;
  }

  // When no decimals have been specified, we allow any. This is used for
  // min/max fields in CCK field settings.
  else if (decimals < 0 && number_parts.length > 1) {
    number += Drupal.settings.format_number.decimal_point + number_parts[1];
  }

  return number;
}

/**
 * Parse a number with (site default or user defined) thousands separator
 * and decimal point.
 *
 * Formatting options are expected to be at Drupal.settings.format_number.
 *
 * @param string number
 *   A number formatted with localized thousands separator and decimal point.
 * @param boolean required
 *   FALSE to return "" when input is empty string. Otherwise, result is always
 *   returned as a valid number. Default is TRUE.
 * @return number
 *   A valid number.
 */
Drupal.parseNumber = function(number, required) {
  if (typeof(number) != "string") {
    return (typeof(number) == "number" ? number : 0);
  }
  if (required == undefined) {
    required = true;
  }

  // Get rid of leading/trailing whitespaces.
  if ((number = number.replace(/^\s+|\s+$/g, "")) == "") {
    return (required ? 0 : "");
  }

  // Extract sign and temporarily remove it from input.
  var is_negative = (number[0] == "-");
  if (is_negative) {
    number = number.substr(1);
  }

  // Remove thousands separators, if any.
  if (Drupal.settings.format_number.thousands_sep.length > 0) {
    var thsep = Drupal.settings.format_number.thousands_sep;
    if (thsep == "\u00A0") {
      thsep += " ";
    }
    number = number.replace(new RegExp("[" + thsep + "]", "g"), "");
  }

  // Translate decimal point, if necessary.
  if (Drupal.settings.format_number.decimal_point != ".") {
    number = number.replace(new RegExp("[" + Drupal.settings.format_number.decimal_point + "]", "g"), ".");
  }

  // Truncate from first non-numeric character (at this point only 0-9 and
  // just one dot are allowed).
  // This should also restore back the sign (if necessary) and convert the
  // string into a pure javascript number (integer or float).
  return number.replace(/^([0-9]*\.?[0-9]+)?.*$/, "$1") * (is_negative ? -1 : 1);
}
