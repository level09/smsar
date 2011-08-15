$Id: README.txt,v 1.47 2009/03/02 22:45:10 davidlesieur Exp $

README file for the Faceted Search Drupal module.


Description
***********

The Faceted Search module provides a search API and a search interface for
allowing users to browse content in such a way that they can rapidly get
acquainted with the scope and nature of the content, and never feel lost in the
data. More than a search interface, this is an information navigation and
discovery tool.

The interface exposes metadata in such a way that users can build their queries
as they go, refining or expanding the current query, with results automatically
reflecting the current query. This interface also combines free-text search,
fully leveraging Drupal's search engine. It avoids complex search forms, and
never offers facets that would lead to empty result sets.

The most obvious metadata for faceted searches is provided by Drupal's taxonomy
module. However, Faceted Search's API allows developers to expose other
metadata, therefore providing more more facets to users for browsing content.

Any of the following cases might prompt you to use Faceted Search:

- Users need to filter content using multiple taxonomy terms at the same time.

- Users want to combine text searches, taxonomy term filtering, and other search
  criteria.

- Users don't know precisely what they can find on your site, or what to search
  for.

- You want to hint users at related content they might not have thought of
  looking for, but that could be of interest to them.

- You want to clearly show users what subject areas are the most comprehensive
  on your site.

- You are trying to discover relationships or trends between contents.

- Your site has too much content for it to be displayed through fixed
  navigational structures, but you still want it to be navigable.

- You want to use a faceted classification
  [http://en.wikipedia.org/wiki/Faceted_classification] because a single
  taxonomic order or a single folksonomy is not suitable or sufficient for your
  content.

- Users often get empty result sets when searching your site.

- You think that "advanced" search forms are not fun to use.


The package
***********

Faceted Search is in fact a bundle of modules.

- Faceted Search: Provides the search framework and API.

- Faceted Search UI: Provides the search user interface.

- Faceted Search Views: Allows to use Views to display the search results.

- Author Facet: Allows users to refine the current search based on content
  author.

- Content Type Facet: Allows users to refine the current search based on content
  type.

- Date Authored Facet: Allows users to refine the current search based on
  content creation date.

- Taxonomy Facets: Allows users to search content through taxonomy. Any
  vocabulary can become a facet that can be used to refine the current search.

- Date Facets Format: Provides formatting options for date-based facets.

- Field Keyword Filter: Allows to perform keyword searches restricted by field
  (requires the Field Indexer module).

Hopefully, many more facets will be developed. The API is meant to make it easy
to implement new facets.


Caution
*******

Faceted Search is database-intensive. If your server can barely keep up with
your traffic, this package will make things worst. Make sure to benchmark
performance before deploying this system on a busy site or on a site with many
thousand nodes (more info: http://drupal.org/node/347952).


Requirements
************

- Drupal 6.x (http://drupal.org/project/drupal).

- MySQL 4.1 (or later version). 

- PHP 5.1 (or later version).


Recommended modules
*******************

- CCK Facets (http://drupal.org/project/cck_facets)
  Exposes Content Construction Kit (CCK) (http://drupal.org/project/cck) fields
  as facets.

- Organic Group Facets (http://drupal.org/project/og_facets)
  Exposes organic groups (http://drupal.org/project/og) as facets.

- Views (http://drupal.org/project/views): In combination with the Faceted
  Search Views module, the Views module can give you tremendous flexibility for
  displaying Faceted Search's results, and even for performing additional
  filtering of the search results. See the "Views integration" topic below for
  more details.

- Field Indexer (http://drupal.org/project/field_indexer): The Field Indexer
  module indexes field data into Drupal's search index. Faceted Search's Field
  Keyword Filter module relies on this data to let users perform keyword
  searches restricted by field.

- jQuery Update (http://drupal.org/project/jquery_update): If you wish to use
  Faceted Search UI's tooltips feature (for showing subcategories when hovering
  over a category in the guided search), it is strongly recommended to install
  the jQuery Update module. Make sure to read that module's installation
  instructions. If you don't use the tooltips feature, Faceted Search UI won't
  use jQuery at all, so in that case you would not need jQuery Update.

- Taxonomy hide (http://drupal.org/project/taxonomy_hide): If you use Faceted
  Search UI's Related Categories block, you might want to remove Drupal's
  default terms listing when viewing a node. You could do that from your site's
  theme, but another way could be to use the Taxonomy hide module.


Known incompatibilities
***********************

- PostgreSQL: PostgreSQL has not actually been tested at this point. Also, the
  Date Authored Facet module uses some MySQL-specific functions.  Feel free to
  share patches to support PostgreSQL (or any other database). :-)
  See http://drupal.org/node/230471 for updates, and remember that you can help
  make PostgreSQL support happen!


Installation
************

1. When using MySQL, in addition to the database permissions already required by
   Drupal, you will also need to grant your database user the LOCK TABLES and
   the CREATE TEMPORARY TABLES permissions. To grant those permissions, you need
   to login to your database:

     mysql -u username -p

   You will be asked for the 'username' database password. Then, at the MySQL
   prompt, enter the following command:

    GRANT LOCK TABLES, CREATE TEMPORARY TABLES
    ON databasename.*
    TO 'username'@'localhost';

   where

     'databasename' is the name of your database
     'username@localhost' is the username of your MySQL account

   Note: Unless your database user has the privileges listed above, you will not
   be able to have Faceted Search work properly.

   If successful, MySQL will reply with:

     Query OK, 0 rows affected

   Then enter the following command:

     quit;

2. Extract the 'faceted_search' module directory, including all its
   subdirectories, into your Drupal modules directory.

3. Go to the Administer > Site building > Modules page, and enable the following
   modules:

   - Faceted Search
   - Faceted Search UI
   - At least one of the following modules (which are provided with Faceted
     Search). Technically, you could use Faceted Search without any of these,
     but there would not be much benefit over Drupal's standard search:
     - Author Facet
     - Content Type Facet
     - Date Authored Facet
     - Taxonomy Facets
     - Field Keyword Filter
   - Search (Drupal core module)
   - Taxonomy (Drupal core module -- only needed if you intend to use Taxonomy Facets)

4. Go to the Administer > Site configuration > Faceted Search page, and click
   the Add Environment tab.

5. Define a faceted search environment by filling the "Add a faceted search
   environment" form. Hopefully it is self-explanatory enough, but don't be
   afraid to experiment. You can always change any of the settings later.

   Click Save to save the new environment. This takes you back to the 
   Administer > Site configuration > Faceted Search page.

6. Go to the Administer > Site building > Blocks page, and enable the following
   blocks (where my_search is the name of the faceted search environment you
   have just created):

   - my_search / Current search
   - my_search / Keyword search
   - my_search / Guided search
   - my_search / Related categories
   - my_search / Sort options

   Use Weight to order the blocks. Having the Current search block located above
   the Keyword search and Guided search blocks is generally most intuitive for
   users.

   When using multiple faceted search environments, you'll want to configure the
   block visibility to avoid showing multiple Keyword search or Guided search
   blocks at the same time. The most common setting is to have those blocks
   visible only on your search environment's base path.

   To do so, in Administer > Site building > Blocks, click Configure next to the
   Keyword search or Guided search block whose visibility is to be adjusted.
   Select "Show on every page except the listed pages", then enter the following
   paths in the Pages field:

   base_path
   base_path/*

   ... where "base_path" should be replaced with your search environment's
   actual base path.

7. Go to the Administer > User management > Access control page, and grant the
   "use faceted search" permission to the roles you intend to give access to
   faceted search.


Views integration
*****************

With the Faceted Search Views module, you may use the Views module to display
and further filter search results.

This system replaces the results page with an embedded view that gets passed the
current search environment as an argument. That argument gets processed by
Faceted Search Views itself through Views' hook_views_query_alter(). You don't
need to configure any particular arguments in your view.

The main requirements for a view to be eligible for use with Faceted Search are:

- Must be a Node view.
- Must use a pager.
- Must be enabled.

To use a view to display search results, go to Administer > Site configuration >
Faceted search, edit your faceted search environment, then select the desired
view in the "Display style" field of the "Results page" section.


Known limitations:

- The view cannot use exposed filters or arguments.
- The view's "Title" and "Empty text" settings are ignored.
- If the view is set to have an unlimited number of items per page, instead of
  having no limit the system's default limit will be used (per Administer >
  Content management > Post settings > Number of posts on main page).


Known issues:

- If your site is using table prefixing, you will need to tell Drupal not to
  prefix temporary tables needed by Faceted Search Views. In settings.php, you
  need something like the following:

    $db_prefix = array(
      'default' => '[your_default_prefix]_',
      'temp_faceted_search_results_[env_id]' => '',
    );

  You will need as many 'temp_faceted_search_results_[env_id]' entries as there
  are faceted search environments (env_id is the numeric identifier for the
  faceted search environment, to be specified without the brackets). You can
  find out the env_id by editing an environment and looking at its administration
  URL, which has a path in the form 'admin/settings/faceted_search/[env_id]'.

  Reference: http://drupal.org/node/227634#comment-864171.


Multilanguage support
*********************

Multilanguage support in Faceted Search relies on the Internationalization
module (http://drupal.org/project/i18n).

With Taxonomy Facets, if a vocabulary uses the 'Localize terms' translation
mode, then the system will display the localized version of the vocabulary's
name and its term names for the current language.


Upgrading from Drupal 5.x
*************************

Before upgrading your site and Faceted Search from Drupal 5 to Drupal 6, make
sure that your site is using the latest version of Faceted Search for Drupal
5.x. If this is not the case, then you will have to perform this update as a
separate step *before* updating to Drupal 6.

Note that Faceted Search Views for Drupal 6 no longer needs an argument (which
was called "Faceted Search: Environment ID") to be added to your views. After
the upgrade to Drupal 6, you will have to remove any remnants of that argument
from your views.


Support
*******

For support requests, bug reports, and feature requests, please use Faceted
Search's issue queue on http://drupal.org/project/issues/faceted_search.

Please DO NOT send bug reports through e-mail or personal contact forms, use the
aforementioned issue queue instead.

For general discussions about Faceted Search (or other Drupal search solutions),
you are invited to join the Search group on http://groups.drupal.org/node/4102.


Credits
*******

- Project initiated by David Lesieur (http://davidlesieur.com,
  http://drupal.org/user/17157).

- Sponsored in part by Laboratoire NT2 (http://www.labo-nt2.uqam.ca), Eyos BV
  (http://www.eyos.nl), and CAIS Institute (http://caisinstitute.org).

- The superb Flamenco search interface (http://flamenco.berkeley.edu) has
  provided much inspiration for this project.
