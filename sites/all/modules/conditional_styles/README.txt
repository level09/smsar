ABOUT CONDITIONAL STYLESHEETS
-----------------------------

Internet Explorer implements a proprietary technology called Conditional
Comments. While web developers frown upon technologies that aren't cross-browser
supported, many CSS developers have found Conditional Comments very useful since
they can be used to fix the broken rendering of CSS in IE by placing IE-only CSS
inside conditional comments.

This module allows themes to easily add conditional stylesheets to the theme's
.info file.


THEME USERS
-----------

You only need to enable this module if a theme requires that you use it. Once it
is enabled, the module automatically performs all of its work for any theme
requiring it. You don't need to configure anything.


THEME DEVELOPERS
----------------

Before this module was available the only way to have IE conditional stylesheets
was to hard-code them into your page.tpl.php. This module allows you to add
"conditional-stylesheets" lines to your theme's .info file.

The syntax for that is:
  conditional-stylesheets[CONDITIONAL][MEDIA][] = stylesheet.css

  where
    CONDITION can be any of the conditions specified in:
      http://msdn.microsoft.com/en-us/library/ms537512.aspx
    MEDIA can be any of the normal CSS media keywords.

For example, to add a stylesheet that only targets IE 6 and below, use:
  conditional-stylesheets[if lt IE 7][all][] = ie6-and-below.css

And to add a print stylesheet for IE8 only, use:
  conditional-stylesheets[if IE 8][print][] = ie8.css


*** IMPORTANT ***

Drupal 6 also stores a cache of the data in .info files. If you modify any lines
in your theme's .info file, you MUST refresh Drupal 6's cache by simply visiting
the admin/build/themes page.
