;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Money CCK field Conversion Dialog for Drupal 6
;; $Id: README.txt,v 1.1.2.1 2008/11/23 18:03:03 markuspetrux Exp $
;;
;; Original author: markus_petrux  (http://drupal.org/user/39593)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

OVERVIEW
========

Provides 'Click to convert!' addon for Money CCK fields, implemented as an
additional formatter for Money CCK fields.

When enabled, a small calculator icon is displayed next to the money value when
the node is rendered. When the calculator icon is clicked, a conversion dialog
pops up allowing the user to choose the currency to convert the amount to.

The conversion dialog is based on jQuery UI Dialog widget, and can be themed to
suit the site needs. The conversion is performed using the Currency API which
is invoked on the background using Ajax when the user selects a currency.

The module allows site administrators grant access to this feature by means of
the 'use money conversion dialog' permission. When a particular user does not
have permission to use the conversion dialog, the default money formatter is
used.


REQUIREMENTS
============

- CCK (http://drupal.org/project/cck)
- Currency API (http://drupal.org/project/currency)
- Format Number API (http://drupal.org/project/format_number)
- Formatted Number CCK (http://drupal.org/project/formatted_number)
- Money CCK fields (http://drupal.org/project/money)
- jQuery UI (http://drupal.org/project/jquery_ui)


INSTALLATION
============

- Please, make sure all required modules are installed first.

- Copy all contents of this package to your modules directory preserving
  subdirectory structure.

- Goto Administer > Site building > Modules to install this module.

- Goto Administer > User management > Permissions and assign the permission
  'use money conversion dialog' to the user roles of your choice.

- Edit the content types that have Money fields, visit the tab 'Display fields'
  and choose the option 'Conversion dialog' for the Money based fields of your
  choice.
