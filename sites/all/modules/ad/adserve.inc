<?php
// $Id: adserve.inc,v 1.1.2.31.2.8.2.10 2009/11/23 19:12:41 Jeremy Exp $

/**
 * @file
 * Configuration.
 *
 * Copyright (c) 2005-2009.
 *   Jeremy Andrews <jeremy@tag1consulting.com>.
 *
 * By default, adserve configuration happens dynamically as ads are served.
 * However, it is possible to override dynamic settings with static defaults.
 * Refer to the documentation/ADSERVE_CONFIGURATION.txt for details on adding
 * adserve overrides to settings.php.
 *
 * Note that the path to Drupal's root directory can not be overriden in
 * settings.php as adserve needs this path to find settings.php in the first
 * place.  To hard code the path to Drupal's root directory, uncomment the
 * following define statement, and set the correct path.  This is not generally
 * required.  On a Unix server this path will be something like '/path/to/web'.
 * On a Windows server this path will be something like 'D:\path\to\web'.
 */
//define('DRUPAL_ROOT', '/var/www/html');

/**
 * The main adserve logic.
 */
function adserve_ad($options = array()) {
  static $displayed_count = 0;

  // if no $options are passed in, assume we're using JavaScript
  if (!empty($options)) {
    adserve_variable('variable_load', $options);
  }
  else {
    adserve_variable('variable_load');
  }

  // include Drupal's settings.php
  adserve_bootstrap(0);

  // if debug enabled, dump current state
  adserve_debug();

  // start with 'error' set to false
  adserve_variable('error', FALSE);

  // invoke cache function (file already included in adserve_variable)
  $ids = adserve_cache('get_ad_ids');

  // display the advertisement(s)
  return adserve_cache('display', $ids);
}

/**
 * Retrieve variables from $_GET array or from passed in $value array.
 */
function adserve_variable($variable, $value = NULL) {
  global $conf;
  static $variables = NULL, $overridden = NULL, $cache_loaded = array();

  // Declare variables if not already declared.
  if ($variables === NULL) {
    $variables = new stdClass();
  }

  // Update the value, if set.
  if (isset($value)) {
    $variables->$variable = $value;
  }

  if (!isset($variables->loaded) || $variable == 'variable_load') {
    if ($variable == 'variable_load' && isset($value)) {
      $values['debug'] = isset($value['debug']) ? $value['debug'] : '';
      $values['c'] = isset($value['adcache']) ? $value['adcache'] : '';
      $values['n'] = isset($value['nids']) ? $value['nids'] : '';
      $values['t'] = isset($value['tids']) ? $value['tids'] : '';
      $values['k'] = isset($value['hostid']) ? $value['hostid'] : '';
      $values['q'] = isset($value['quantity']) ? $value['quantity'] : 1;
      $values['m'] = isset($value['ad_display']) ? $value['ad_display'] : 0;
      unset($value);
    }
    else {
      $values = $_GET;
    }

    // Don't use getcwd as path may involve symbolic links
    $variables->ad_dir = dirname($_SERVER['SCRIPT_FILENAME']);
    // 'debug' is an integer.
    $variables->debug = isset($values['debug']) ? (int)$values['debug'] : 0;
    // Cache types are comprised of only letters.
    $variables->adcache = isset($values['c']) ? preg_replace('/[^a-zA-Z]/', '', $values['c']) : 'none';
    // Nids is an integer or a ",".
    $variables->nids = isset($values['n']) ? preg_replace('/[^0-9,]/', '', $values['n']) : '';
    // Tids is an integer or a ",".
    $variables->tids = isset($values['t']) ? preg_replace('/[^0-9,]/', '', $values['t']) : '';
    // Hostid is an md5() which is comprised of numbers and letters a-f.
    $variables->hostid = isset($values['k']) ? preg_replace('/[^0-9a-f]/', '', $values['k']) : '';
    // Display url
    $variables->url = isset($values['u']) ? $values['u'] : '';
    if (!$variables->url) {
      $variables->url = isset($_SERVER['HTTP_REFERER']) ? $_SERVER['HTTP_REFERER'] : '';
    }
    // Display alias
    $variables->alias = isset($values['l']) ? $values['l'] : '';
    // Quantity is an integer.
    $variables->quantity = isset($values['q']) ? (int)$values['q'] : 0;
    // Ad ID is an integer.
    $variables->aid = isset($values['a']) ? (int)$values['a'] : 0;
    // Method is compriese of only letters.
    $variables->ad_display = isset($values['m']) ? preg_replace('/[^a-zA-Z]/', '', $values['m']) : 'javascript';

    // Set defaults.
    $variables->quantity = $variables->quantity ? $variables->quantity : 1;

    if ($variables->debug) {
      foreach ($variables as $variable => $val) {
        echo "$variable: '$val'<br />\n";
      }
      if ($variables->debug == 1) exit;
    }
    $variables->loaded = TRUE;

    // Override the value, if set during initialization.
    if (isset($value)) {
      $variables->$variable = $value;
    }
  }

  if (!$overridden) {
    if (isset($conf)) {
      foreach ($conf as $var => $val) {
        $variables->$var = $val;
        if ($variables->debug) {
          echo "Override $var: '$val'<br />\n";
        }
      }
      $overridden = TRUE;
    }
  }

  if (!isset($cache_loaded[$variables->adcache])) {
    // Retrieve variables defined by cache plugin, if enabled.
    if ($variables->adcache != 'none') {
      $includes = array($variables->ad_dir ."/cache/$variables->adcache/ad_cache_$variables->adcache.inc", $variables->ad_dir ."/../ad_$variables->adcache/ad_cache_$variables->adcache.inc");
      foreach ($includes as $include) {
        if (file_exists($include)) {
          if ($variables->debug) {
            echo "Attempting to include cache include file '$include'.<br />\n";
          }
          require_once($include);
        }
        else if ($variables->debug) {
          echo "Failed to find cache include file '$include'.<br />\n";
        }
        $function = 'ad_cache_'. $variables->adcache .'_variables';
        if (function_exists($function)) {
          $external_variables = $function();
          foreach ($external_variables as $key => $val) {
            if (!isset($variables->$key)) {
             $variables->$key = $val;
           }
         }
       }
      }
    }
    $cache_loaded[$variables->adcache] = TRUE;
  }

  if ($variable == 'variable_dump') {
    echo "Dumping \$variables:<br />\n";
    echo '<pre>';
    foreach ($variables as $var => $val) {
      echo "  $var($val)<br />\n";
    }
    echo '</pre>';
  }

  if (isset($variables->$variable)) {
    return $variables->$variable;
  }
  else {
    return NULL;
  }
}

/**
 * Invoke a function in the specified file.
 */
function adserve_invoke_file($function, $arg1 = NULL, $arg2 = NULL) {
  $output = '';
  if (function_exists($function)) {
    $output = $function($arg1, $arg2);
  }
  else if (adserve_variable('debug')) {
    echo "Function '$function' does not exist.<br />\n";
  }
  return $output;
}

/*
 * When debugging, strip away distracting header errors.  Dump all other errors.
 */
function _debug_error_handler($errno, $errstr, $errfile = NULL, $errline = 0, $errcontext = NULL) {
  if (!preg_match('/Cannot modify header information/', $errstr) &&
      !preg_match('/Cannot send session cache limiter/', $errstr)) {
    echo "PHP: errno($errno): $errstr ";
    if ($errfile && $errline) {
      echo "; Line $errline in [$errfile]";
    }
    echo "<br />\n";
    if (!empty($errcontext) && adserve_variable('debug') >= 5) {
      echo 'Error context:<pre>';
      print_r($errcontext);
      echo '</pre>';
    }
  }
}

/**
 * Dump debug message to screen; set custom error handler.
 */
function _debug_echo($text) {
  static $error_handler = FALSE;
  static $time = 0;

  if (adserve_variable('debug')) {
    if ($time < time()) {
      $time = time();
      echo '--> Time mark: '. date('H:i:s', $time) ."<br />\n";
      _debug_memory();
    }
    if (!$error_handler) {
      set_error_handler('_debug_error_handler');
      $error_handler = TRUE;
    }
    echo "$text<br />\n";
  }
}

function _debug_memory() {
  $memory = '';
  if (adserve_variable('debug') && function_exists('memory_get_usage')) {
    $memory = number_format(round(memory_get_usage() / 1024, 3), 3);
    echo "Memory usage: $memory K<br />\n";
  }
}

/**
 * Include Drupal's bootstrap.inc.
 */
function adserve_include_drupal() {
  // For optimal performance set DRUPAL_ROOT at the top of this file.
  if (defined('DRUPAL_ROOT')) {
    if (is_dir(DRUPAL_ROOT) && file_exists(DRUPAL_ROOT .'/includes/bootstrap.inc')) {
      chdir(DRUPAL_ROOT);
      adserve_variable('root_dir', DRUPAL_ROOT);
    }
    else {
      echo 'Invalid DRUPAL_ROOT ('. DRUPAL_ROOT .') defined in adserve.inc';
    }
  }
  else {
    $path = explode('/', str_replace('\\', '/', adserve_variable('ad_dir')));
    while (!empty($path)) {
      // Search for top level Drupal directory to perform bootstrap.
      chdir(implode('/', $path));
      if (file_exists('./includes/bootstrap.inc')) {
        adserve_variable('root_dir', getcwd());
        break;
      }
      array_pop($path);
    }
  }
  require_once adserve_variable('root_dir') .'/includes/bootstrap.inc';
}

/**
 * Include the necessary files and call the Drupal bootstrap.
 */
function adserve_bootstrap($bootstrap = NULL) {
  adserve_include_drupal();

  // If no specific bootstrap is specified, do a full bootstrap.
  if (!isset($bootstrap)) {
    $bootstrap = DRUPAL_BOOTSTRAP_FULL;
  }

  echo _debug_echo("Drupal bootstrap '$bootstrap'.");

  drupal_bootstrap($bootstrap);
  echo _debug_echo("Drupal bootstrap complete.");
}

/**
 * Display additional debug information.
 */
function adserve_debug() {
  if (adserve_variable('debug')) {
    echo "Root drupal directory detected as '". adserve_variable('root_dir') ."'.<br />\n<br />\n";

    $ad_dir = adserve_variable('ad_dir');
    $files = array("$ad_dir/serve.php", "$ad_dir/adserve.inc", "$ad_dir/adcache.inc", "$ad_dir/ad.module");
    if (adserve_variable('debug') >= 2) {
      $files = array_merge($files, array("$ad_dir/ad.install"));
    }
    if (adserve_variable('debug') >= 3) {
      $files = array_merge($files, array("$ad_dir/image/ad_image.module", "$ad_dir/image/ad_image.install", "$ad_dir/text/ad_text.module", "$ad_dir/text/ad_text.install", "$ad_dir/embed/ad_embed.module", "$ad_dir/report/ad_report.module", "$ad_dir/notify/ad_notify.module", "$ad_dir/notify/ad_notify.install", "$ad_dir/cache/file/ad_cache_file.inc", "$ad_dir/cache/file/ad_cache_file.module", "$ad_dir/permission/ad_permission.module", "$ad_dir/weight/probability/ad_weight_probability.module", "$ad_dir/weight/probability/ad_weight_probability.inc"));
    }
    foreach ($files as $file) {
      if (!file_exists($file)) {
        echo "Error: '$file' does not exist!<br />\n";
      }
      else if (!is_readable($file)) {
        echo "Error: '$file' is not readable!<br />\n";
      }
      else {
        $fd = fopen($file, 'r');
        while (!feof($fd)) {
          $line = fgets($fd);
          if (substr($line, 0, 5) == "<?php") {
            continue;
          }
          else {
            echo "$file: $line<br />";
            break;
          }
        }
      }
    }
    echo "<br />\n";
  }
}

