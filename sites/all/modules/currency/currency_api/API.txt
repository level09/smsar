The currency module provides two callable API functions that allow other
modules to do real time currency exchange.

The functions are:

function currency_api_convert($currency_from, $currency_to, $amount = 1)

 * This function converts two currencies using exchange rates from Yahoo Finance.
 * The currency codes are standard ISO 3-letter codes, and you can find the details
 * here:
 *  http://www.oanda.com/site/help/iso_code.shtml
 *
 * Here is an example on how to use it:
 *
 *   $from = 'CAD';
 *   $to   = 'USD';
 *   $amt  = 20;
 *   $ret  = currency_api_convert($from, $to, $amt);
 *   if ($ret['status'] == FALSE) {
 *     drupal_set_message(t('An error occured: '). $ret['message']);
 *   }
 *   else {
 *     print $amt . ' ' . $from . ' = ' . $ret['value'] . ' ' . $to;
 *   }
 *
 * @param $currency_from
 *   Currency to convert from.
 * @param $currency_to
 *   Currency to convert to.
 * @param $amount
 *   (optional) Amount to convert. Defaults to 1.
 * @param $decimals
 *   (optional) Number of digits to the right of the decimal point. Leave out this
 *   parameter if you want the actual currency result to proceess it yourself.
 *   Defaults to NULL.
 *
 * @return $result
 *   An associative array that contains the following:
 *    $result['status'] - TRUE or FALSE
 *    $result['message'] - 'success' when status is TRUE, otherwise, contains a
 *                         descriptive error text
 *   The following items are only returned when status is TRUE
 *    $result['value'] - $amount * exchange rate of $currency_from into $currency_to
 *    $result['rate'] - Exchange rate of $currency_from into $currency_to
 *    $result['timestamp'] - Timestamp of the last update to the rates
 *    $result['date'] - Date of the last update to the rates (Format is "m/d/yyyy")
 *    $result['time'] - Time of the last update to the rates (Format is "h:mmpm")

function currency_api_get_desc($currency)

 * This function gets the currency name for a standard ISO 3-letter codes,
 * You can find the details here:
 *  http://www.oanda.com/site/help/iso_code.shtml
 *
 * Here is an example on how to use it:
 *
 *   $ccode = 'CAD';
 *   $ret = currency_get_description($ccode);
 *   if ($ret == FALSE)
 *   {
 *     drupal_set_message(t('Could not get description'));
 *   }
 *   else
 *   {
 *     print $ccode .' => '. $ret;
 *   }
 *
 * @param $currency
 *   Currency code (3-letter ISO)
 *
 * @return $result
 *   Contains FALSE if the currency cannot be found, otherwise, it
 *   has the description.

function currency_api_get_symbol($currency)

 * This function gets the currency symbol for a standard ISO 3-letter codes,
 * You can find the details here:
 *  http://www.oanda.com/site/help/iso_code.shtml
 *
 * Here is an example on how to use it:
 *
 *   $ccode = 'CAD';
 *   $ret = currency_api_get_symbol($ccode);
 *   if ($ret == FALSE) {
 *     drupal_set_message(t('Could not get symbol'));
 *   }
 *   else {
 *     print $ccode .' => '. $ret;
 *   }
 *
 * @param $currency
 *   Currency code (3-letter ISO)
 *
 * @return $result
 *   Contains FALSE if the currency symbol cannot be found, otherwise, it
 *   has the symbol.

function currency_api_get_list()

 * Returns a list of supported currencies
