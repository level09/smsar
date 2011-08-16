---------
Overview:
---------
The ad module is a powerful advertising system for Drupal-powered websites. It
supports the random display and tracking of graphical (banner), text and raw
html ads.  Ads can easily be displayed in themes, blocks, or embedded in site
content. The module records comprehensive statistics about when and how often
ads are viewed and clicked, including a plug-in module for generating graphical
time-based reports. Ads can be assigned to multiple owners, each of which can
be assigned their own set of permissions.  Installation is simple by design. An
API is provided allowing the development of additional functionality and
integration with other Drupal modules.

Features:
    * auto-generated ad blocks supporting a configurable number of ads
    * automatically or manually embed ads into site content
    * collection of comprehensive statistics allowing time-based reporting and
      analysis
    * tracking of when and where ads are clicked, by which user and which IP
    * advertisements can have multiple owners
    * granular per-advertisement/per-owner permissions system
    * activation/expiration scheduling based on time, clicks or impressions
    * an ad_image plug-in for image (aka banner) ads
    * an ad_text plug-in for simple text ads
    * an ad_html plug-in for raw html ads
    * an externally maintained ad_geoip plug-in provides support for
      geotargeting ads
    * an ad_report plug-in for basic graphical reports
    * an ad_notify plug-in for scheduling automatic email notifications
    * an ad_remote plug-in for hosting ads on remote (non-Drupal) websites
    * an administrative statistics overview page
    * support for any number of configurable ad groups, utilizing Drupal's
      taxonomy (category) subsystem
    * display ads based on node ids (nids) or taxonomy terms (categories)
    * file-based caching for improved performance
    * memcache-based caching for improved performance
    * support for external caching methods
    * MySQL and PostgreSQL support


------
Usage:
------
Installation and requirements information can be found within the INSTALL.txt
file included with this module.


-------------
Creating ads:
-------------
Once the ad module is properly installed an enabled, you can create
advertisements by visiting 'create content' on your website and choosing
'advertisement' as the content type.  If you have enabled multiple ad type
modules, you can select one of them from the resulting menu, such as 'image
advertisement' or 'text advertisement'.

  Text ads:
  ---------
  Text ads are very simple, requiring that you fill out only three fields of
  information.  First, you need to specify the Destination URL where you want
  people to be redirected when they click on your text ad.  Second, you enter
  the a header for your ad.  The header will be linked to the Destination URL.
  Finally, you need to specify the body of your ad.

  Image ads:
  ----------
  Image ads or only slightly more complicated than text ads.  They depend on
  the drupal core Upload module for managing the actual images.  As with text
  ads, you first need to specify the Destination URL where you want people to
  be redirected when they click on your image ad.  Second, you can optionally
  enter some Mouseover text.  If you enter text in the mouseover field this
  text will be displayed when people hover their mouse pointer over the
  advertisement.  Finally, you need to scroll down to the File attachements
  section of the page and click the Browser button to select your image, then
  click Attach to upload the image.  If you upload multiple images, the first
  image with "List" checked will be displayed.  If no images have "List" checked
  then no images will be displayed.

  HTML ads:
  ---------
  HTML ads allow you to easily define your own custom ad type by simply pasting
  in a block of HTML code.  At this time, however, the ad module is unable to
  track when html ads are clicked.

  Ad groups:
  ----------
  Ads can be organized into groups.  For example, you may have a group called
  "Text Ads" and another group called "Image Ads."  You could then assign your
  text ads to the "Text Ads" group, and your image ads to your "Image Ads"
  groups.  (This is not required, it is perfectly valid to include both image
  ads and text ads in the same group.)  When displaying ads, you typically tell
  the ad module to display ads from a certain group and then the ad module
  randomly selects an active ad from the specified group.  Each ad can be a
  member of any number of groups.

  Ad status:
  ----------
  There are several states that an ad can exist in.  An ad in the Pending state
  is one that has recently been uploaded and is waiting to be approved by a
  privileged user.  An ad in the Approved state is one that has been approved
  by a privileged user but is not actively being displayed, the ad could be
  waiting on an autoactivation event.  An ad in the Active state is being
  actively displayed.  An ad in the Offline state is approved but is currently
  not being displayed.  An ad in the Unpublished state means that the ad node
  was unpublished so the ad is not any longer being displayed.  An ad in the
  Expired state is no longer being displayed.  An ad in the Denied state means
  it was not approved by the site administrator.

  Scheduling:
  -----------
  If you put an ad into the Approved state and then enter a date and time in the
  Automatically Activate Ad field, the ad will be automatically activated
  on the date and time specified.  This feature requires that cron be functional
  on your website.  If you enter a date and time in the Automatically Expire Ad
  field, the ad will be automatically expired on the date and time specified.
  Again, this feature requires that cron be functional on your website.

  If you enter a number into the Maximum Impressions field, the ad will be
  automatically expired once it has been displayed this number of times.

  If you enter a number into the Maximum Clicks field, the ad will be
  automatically expired once it has been clicked this number of times.

---------------
Displaying ads:
---------------
There are many ways to display ads with the ad module.  The simplest way
is to enable one of the automatically generated ad blocks.  It is also possible
to use the ad_embed module to automatically embed ads within your site content.
And you can even display ads from within the PHP of your theme or another
module.

  ----------
  Ad blocks:
  ----------
  The ad module automatically generates on ad block for every ad group that you
  create.  For example, if you visit "Administer >> Site building >> Blocks"
  you will find a block named "ad group: default".  If you enable this block,
  it will display a random ad from all active ads in the default group.

  You can optionally configure the block to display more than one ad at a time
  by clicking the 'configure' link on the block administration page.

  -------------
  Embedded ads:
  -------------
  If you enabled the ad_embed module, it is possible to embed ads into your
  site content.  To configure the ad_embed module go to "Administer >> Content
  Management >> Ads >> Settings >> Embedded ads".

  If you wish to manually embed ads in your content, check the box next to
  "Replace ad bracket tags" or "Replace ad comment tags".  This will cause the
  ad_embed module to replace embedded [[add]] or <!--ad--> tags respectively.
  Instructions on how to specify specific ad groups or even specific ads and
  how many ads to display at a time can be found by visiting "Administer >>
  Help >> Embed".

  If you wish to automatically embed ads in your content, configure this on a
  per-content-type basis in the lower configuration section.
  embedded ads".

  ------------------------
  Displaying ads from PHP:
  ------------------------
  To display an ad from within PHP code, make a call to the ad() function.
  You can optionally specify an ad group, the number of ads to display, and
  several other options.  For example, to display one random ad from all ads
  that have not been assigned to any group you don't have to pass in any
  parameters:
      <?php print ad(); ?>

  To display two ads from all ads that have not been assigned to any group
  you would execute the following code:
      <?php print ad(0, 2); ?>

  (The first parameter specifies the ad group to display ads from.  By
  specifying 0, we are telling the ad module to display ads that are not
  assigned to any group.  The second parameter specifies the number of ads
  to display at one time.)

  To randomly display an ad from a specific group of nids, for example with
  the node ID 69 or 76, you would pass in the following parameters:
      <?php print ad(NULL, 1, array('nids' => '69,76')); ?>

  (When specifying specific nids, any specified ad group is ignored, so we
  leave the first parameter as NULL.  The second parameter causes only one
  ad to be displayed at a time.  And the third parameter is an array that
  tells the ad module to randomly display either ad 69 or ad 76.)

  To display and ad randomly selected from multiple groups you can simply
  specify multiple groups separated by commas.  For example, to display 3
  ads from groups 24, 56 and 98 you would pass in the following parameters:
      <?php print ad('24,56,98', 2); ?>

  You can also specify how to display a given ad.  Current display methods are
  'javascript', 'jquery', 'iframe', and 'raw'.  When using the 'javascript',
  'jquery', and 'iframe' methods, ads will randomly change even when the Drupal
  pagecache is enabled.  When using the 'raw' method, ads will only change when
  the Drupal pagecache is flushed.

  To force one ad with a tid of 76 to display using JavaScript you would pass
  in the following parameters:
      <?php print ad(76, 1, array('method' => 'javascript'));

  To force two ads with a tid of 101 or 102 to display using the Raw method
  you would pass in the following parameters:
      <?php print ad('101,102', 76, array('method' => 'raw'));

  ------------
  Theming ads:
  ------------
  All ads are wrapped in the following tags:
    <div class="advertisement" id="group-#"></div>

  All ads are in the same "advertisement" class.  Each ad group gets a unique
  id.  This makes it possible to create generic advertisement formatting as
  well as specific advertisement formatting.

    CSS Example 1:
    --------------
    This sample code can be added to your theme's custom style.css.  It adds
    padding to advertisements, wrapping them in a dashed border and giving
    them a background color.  It also adds the text "Advertisement:" above
    the ad:

       .advertisement {
         padding: 5px;
         border: dashed;
         background-color: #ffd;
       }

       .advertisement:before {
         content: "Advertisement:";
       }

    CSS Example 2:
    --------------
    Here is more sample code that could be added to your theme's custom
    style.css.  It will cause multiple image ads to be displayed horizontally
    one beside the other (space permitting), rather than vertically one
    below the other.  The ads are aligned to the left side of the screen,
    if you'd prefer them to be aligned to the right side of the screen change
    the word "left" to "right" in the snippet:

       .image-advertisement {
         float: left;
         padding: 3px;
       }


-------------------
Ad module settings:
-------------------
The ad module and related modules provide a number of configuration options.

  ----------------
  Global settings:
  ----------------
  The ad module allows you to specify some global settings at "Administer >>
  Content management >> Ads >> Settings >> Global settings".

  Even if you do not plan to make any changes, you should visit this page at
  least one time as the ad module will perform a series of sanity tests when
  you visit this page, alerting you to any installation problems.

  This page is divided into sections.  The "Status" section validates that the 
  module can find the necessary scripts for serving advertisements.  If 
  everything is properly installed, it will show you the automatically detected
  paths to serve.php and adserve.inc.  If you instead see errors in this section,
  review the directions in INSTALL.txt to get the ad module properly installed.

  In the "General" section you can configure what happens when an advertisement
  is clicked by setting the 'Click-through target'.  You can also enable the
  'nofollow' option to add 'rel="nofollow"' to all links generated by the ad
  module.  The 'Display type' allows you to alter how advertisements are 
  displayed, using JavaScript, jQuery, IFrames, or simply embedding raw HTML
  into your page.  Regardless of the 'Display type' the ad module will
  accurately track how many times each advertisement is displayed and clicked.
  Finally, the 'Validate URLs' option tells the module whether or not it
  should use Drupal's built in URL validation logic to ensure that the
  Destination URL for new advertisements is a valid URL.

  In the "Search" section there are two options which are enabled by default.
  The first causes advertisements to be removed from local searches when using
  Drupal's built in search module.  The second causes advertisements to be
  removed from remote search engines by adding the "noindex" meta tag to all
  pages where node ads are displayed.  If you publish advertisement nodes to
  the front page mixed in with other content types, you should probably
  disable the "Remove ads from remote search engines" option as otherwise
  you may find that your entire website is removed from all search engines.
  (Note that displaying ads on the front page is not the same as displaying
  an ad node on the front page. The 'noindex' tag is not added when ads are
  displayed via blocks, direct calls to ad(), using views, or when embedded
  with the ad_embed module.)

  The "IFrame" section allows you to configure the size of IFrames and other
  related settings.

  The "Cache" section provides access to the configuration options for any
  enabled ad caching modules.  By default the ad module comes with the
  file cache which allows advertisements to be quickly and efficiently displayed
  from a cache file without bootstrapping Drupal.  If your website utilizes
  multiple web servers, you should instead download the ad_memcache module
  which allows you to serve advertisements from a shared memcache instance.

  ---------
  Text ads:
  ---------
  The ad_text module allows you to specify some minimum and maximum lengths for
  text ads at "Administer >> Content management >> Ads >> Settings >> Text ads".

  ----------
  Image ads:
  ----------
  The ad_image module allows you to specify some image constraints on a
  per-group basis at "Administer >> Content management >> Ads >> Settings >>
  Image ads".

  -------------
  Embedded ads:
  -------------
  Manually and automatically embedded ads can be configured at "Administer >>
  Content management >> Ads >> Settings >> Embedded ads".


-----------
Statistics:
-----------
The ad module tracks how often each ad is viewed and clicked.  The statistics
are tracked to an hourly granularity.

When an advertisement is first enabled, the statistics for 'This hour', 'Today',
'Last seven days', 'This month', 'This year' and 'All time' will all be the
same.  When available, statistics will also include 'Last hour', 'Yesterday',
'Last month', and 'Last year'.

When displaying advertisements using the JavaScript, jQuery, and IFrame methods
impressions are tracked each time the script is loaded.  When displaying
advertisements using the Raw method, impressions are tracked through the use
of a hidden 1x1 pixel image that is displayed along with each advertisement.

The click_filter module can be enabled to filter out the following invalid
clicks:
    * Duplicate clicks from the same IP address
    * Clicks from the owner of the advertisement
    * Clicks from any user with 'filter clicks' permissions.
    * Clicks from any users in which the word "bot" is contained in their
      HTTP_USER_AGENT

This means that when the click_filter module is enabled, all clicks by UID 1
will be filtered because Drupal will automatically assign all permissions to
this user, including the 'filter clicks' permission.  The click_filter module
currently has no configuration options beyond the 'filter clicks' permission.
