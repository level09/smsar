## Upgrading from _Similar By Terms 1_ to _Similar By Terms 2_ ##

Similar By Terms 2 (SBT2) is a Views plug-in, so it works quite differently from Similar By Terms 1 (SBT1). Please read this document thoroughly before upgrading your site. Here are the steps to go through in order to upgrade your site if you were previously using SBT1:

1. Make note of which SBT1 blocks you have enabled, the region assignments, and the settings for each. Write 'em on a piece of paper! You're going to want to remember how you had things set up.
2. Make sure that you have Views installed and enabled.
3. Download SBT2 and replace your existing "similarterms" module directory.
4. Visit update.php and run the update to clear the block cache and remove any cached SBT1 blocks from your site.
5. Visit admin/build/block and assign the "similarterms: Block" block to the region where you had your SBT1 assigned.

If you were using only the "Similar entries from ANY vocabulary" block, then you can jump below to find out how to map your settings. Otherwise, you'll want to visit admin/build/views and edit the "similarterms" view.

If you were previously using one of SBT1's "Similar entries from the {foo} vocabulary" blocks, then you will probably want to edit the view's *Arguments > Similar By Terms: Nid* configuration and change the "Limit similarity to terms within these vocabularies" setting to match that of {foo}.

If you had multiple "Similar entries from the {foo} vocabulary" blocks enabled then you can create multiple block displays within your view, set the *Arguments* section to override the default display, and then set up blocks with the necessary vocabulary limitations.

### Mapping settings from SBT1 to SBT2 ###

Settings for blocks in SBT1 are found at admin/build/block/configure/similarterms/0 and similar. For the SBT2 blocks, most of these settings are changed using Views. Here's the list of SBT1 options and how they map to Views:

* __Block title:__ This can either be set in the admin/build/block options or by changing the view's *Basic settings > Title* configuration
* __Item count:__ This is changed in the view's *Basic settings > Items to display* configuration
* __Show current node as active in list:__ Visit the view's *Arguments > Similar By Terms: Nid* configuration and change the "Include argument node(s) in results"
* __Content type limit:__ Add a filter to the view to limit by "Node: Type"
