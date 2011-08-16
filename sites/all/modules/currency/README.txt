
Copyright 2005 http://2bits.com

Description:
------------

This module provides visitors to your web site with currency conversion rates.

It also provides a callable API for currency conversion that other modules can
use.

It relies on Yahoo! Finance for getting the currency exchange data.

Features:
---------

This module provides several options that customize its look and feel:

* An overview section on the top of the page can contain any text you want

* Users can enter an amount, and that amount will calculated in the target
  currency.

* Provides an input format filter that converts currency tokens like:
  [currency:from:to:value:decimals] to a currency exchange rate.
  The 'decimals' parameter is optional.
  Example: [currency:EUR:USD:100:2].

* Provides a link to a detailed history and chart page on Yahoo Finance.

* All currency conversion operations are logged to the watchdog (optional).

* Provides a callable API for other modules to do currency exchange calculations.
  See currency_api/API.txt for details.

Database:
---------
No database changes are required for this module.

Installation:
-------------
Please see the INSTALL.txt document for details.

Bugs/Features/Patches:
----------------------
If you want to report bugs, feature requests, or submit a patch, please do so
at the project page on the Drupal web site.
http://drupal.org/project/currency

Author
------
Khalid Baheyeldin (http://baheyeldin.com/khalid and http://2bits.com)

Maintainer
----------
Andrei Mateescu (http://drupal.org/user/729614)

If you use this module, find it useful, and want to send the author
a thank you note, then use the Feedback/Contact page at the URL above.

The author can also be contacted for paid customizations of this
and other modules.
