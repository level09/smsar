# SMSAR ! Easy property management made with drupal 6 #

Manage a property website with drupal, easy


## How to install ##

1. Copy the files into the root of your website directory 
2. Create a mysql database using a GUI tool like phpmyadmin or using the command line : 
<pre> create database db_name default charset utf8; </pre>
3. restore the database file in the db directory
4. Open sites/default directory and copy the default.settings.php or rename it to settings.php and update the database configuration line. 
5. make sure the following directory is writable : 
sites/default/files
sites/all/modules/print/lib/images/
sites/all/modules/print/lib/cache/
6. Go to your website url (example.com/user), then login with the following : 

user: admin
pass: admin 

7. Check the status report for any issues. 

