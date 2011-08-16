Author Contact
http://drupal.org/project/authorcontact
================


DESCRIPTION
------------
Creates a block that shows a contact form that when submitted, sends a simple contact email to the author of the current node. This might be useful for a multi-user blog or auction site, for example.


REQUIREMENTS
------------
Drupal 6.x


INSTALLING
----------
1. Copy the 'authorcontact' folder to your sites/all/modules directory.
2. Go to Administer > Site building > Modules. Enable the module.


CONFIGURING AND USING
---------------------
1. Go to Administer > Administer > Site building > Blocks. Drag-and-drop the block 'Author Contact' to a visible region.

2. Click on SAVE BLOCKS button.

3. Click on CONFIGURE link beside the block 'Author Contact'. 

Although the block will automatically only show on nodes, you should configure it to only show on the nodes you want - eg only on blog content types or certain URLs.

From the block configuration page you can also set text to appear at the top of the form. This could give a short instruction or reminder to include your phone number, etc.

4. It is also suggested you install the Captcha module to help prevent spam going to your node authors - see http://drupal.org/node/311349 for some help with this.

5. Please note that this module now takes notice of each user's setting for 'personal contact form' in their profile. If a user has not allowed personal contact, the Author Contact form will not appear on their nodes.


REPORTING ISSUES. REQUESTING SUPPORT. REQUESTING NEW FEATURES.
--------------------------------------------------------------
Feedback and feature requests are welcome.

1. Go to the module issue queue at http://drupal.org/project/issues/authorcontact?status=All&categories=All
2. Click on CREATE A NEW ISSUE link.
3. Fill out the form.
4. To get a status report on your request go to http://drupal.org/project/issues/user


UPGRADING
---------
1. One of the most IMPORTANT things to do BEFORE you upgrade, is to backup your site's files and database. More info: http://drupal.org/node/22281
2. Disable actual module. To do so go to Administer > Site building > Modules. Disable the module.
3. Just overwrite (or replace) the older module folder with the newer version.
4. Enable the new module. To do so go to Administer > Site building > Modules. Enable the module.
5. Run the update script. To do so go to the following address: www.yourwebsite.com/update.php
Follow instructions on screen. You must be log in as an administrator (user #1) to do this step.

Read more about upgrading modules: http://drupal.org/node/250790


CREDITS
-------
This module is created and maintained by James Crook of Choc Chip multimedia

http://www.chocchip.com.au
http://drupal.org/user/204495
