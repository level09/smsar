
Cache hook format:
  $cache['hook_name'] = array(
    'weight' => integer, // optional
    'file' => '/full/path/to/include/file',
    'function' => 'function_name',
  );

Supported hooks:

 - hook_init
    Alter which advertisements are selecting from.

 - hook_filter
    Filter out invalid advertisements.

 - hook_select
    Alter how advertisements are selected.

OLD README (this may not be accurate anymore):
TODO: Update.
Ad caches are defined through external modules.  Ad caches are composed of a
module 'ad_cache_TYPE.module' and an include file 'ad_cache_TYPE.inc' that live
in the 'cache/TYPE' subdirectory where 'TYPE' is replaced with the type of
cache.  For example, the included file cache lives in 'cache/file'.

The ad_cache_TYPE.inc file must have a function named ad_cache_TYPE() which is
used to display ads.  It can optionally include a function titled
ad_cache_TYPE_variables used to extract any necessary variables from the global
$_GET array (this can also be used to override values that would normally be
set from $_GET).  Any functions used by this code without bootstrapping Drupal
should also be in this file.

The ad_cache_TYPE.module file should define the drupal _help() hook so the
module can be enabled.  It should also define the _adcacheapi() hook allowing
for configuration and processing.  Any functions used by this code after
bootstrapping Drupal should also be in this module.

Refer to cache/file/* for an implementation example.
