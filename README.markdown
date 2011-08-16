# SMSAR ! Easy property management made with drupal 6 #

Manage a property website with drupal, easy

## Features 

** Custom Fields for property, built in fields for title, description, price, location, rooms, type, status, size, broker reference 

** Support for images (server side processing and custom actions)
** PDF version, send to friend 
** Custom formatters for fields 
** simple and easy google maps support
** Advanced and simple search 
** Guided search (faceted search module)
** Taxonomy manager, and hierarchal select 
** Theme provided is based on Blueprint css framework, nice layout and typography / forms etc ..
** Primary links (main menu) is based on the drop down css framework
** Contact form 
** mailing list (simple news)

## How to install 

1. Copy the files into the root of your website directory 
2. Create a mysql database using a GUI tool like phpmyadmin or using the command line : 
	create database db_name default charset utf8; 
3. restore the database file in the db directory
this can be done with : 

	mysql -u username -p db_name < db/db 

if you have drush installed, you can simply 

	drush sql-cli < db/db 

4. Open sites/default directory and copy the default.settings.php or rename it to settings.php and update the database configuration line. 
5. make sure the following directory is writable : 
sites/default/files
sites/all/modules/print/lib/images/
sites/all/modules/print/lib/cache/
6. Go to your website url (example.com/user), then login with the following : 

user: admin
pass: admin 

7. Check the status report for any issues. 

## Demo site 

http://smsar.me 

