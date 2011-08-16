;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Formatted Number CCK module for Drupal
;; $Id: README.txt,v 1.1.2.4 2009/09/20 11:07:02 markuspetrux Exp $
;;
;; Original author: markus_petrux (http://drupal.org/user/39593)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

CONTENTS OF THIS FILE
=====================
* OVERVIEW
* REQUIREMENTS
* INSTALLATION
* ADDITIONAL FORMATTERS


OVERVIEW
========

The Formatted Number CCK module defines numeric types where thousands separator
and decimal point are inherited from the Format Number API module.

Numeric types supported: signed or unsigned integers (tiny, small, medium, int),
decimal and float.

Integrates with:
- Diff (http://drupal.org/project/diff)


REQUIREMENTS
============

- CCK (http://drupal.org/project/cck)
- Format Number API (http://drupal.org/project/format_number)


INSTALLATION
============

- Please, make sure all required modules are installed first.

- Copy all contents of this package to your modules directory preserving
  subdirectory structure.

- Goto Administer > Site building > Modules to install this module.

- Create or edit content types and start adding Money fields. :)


ADDITIONAL FORMATTERS
=====================

You can easily add additional formatters adding the following lines to your
settings.php file:

<code>
// Additional formatters for fixed number of decimal places.
$conf['formatted_number_decimal_place_formatter_lengths'] = array(0, 1, 2);

// Additional formatters for fixed number of significant figures.
$conf['formatted_number_significant_figure_formatter_lengths'] = array(1, 2, 3, 4);
</code>
