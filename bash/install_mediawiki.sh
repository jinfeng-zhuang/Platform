#!/bin/sh

sudo apt install apache2 mysql-server-5.7 php php-mysql php-mbstring php-xml
sudo mysql_secure_installation
sudo /etc/init.d/apache2 restart
wget https://releases.wikimedia.org/mediawiki/1.30/mediawiki-1.30.0.tar.gz
tar -xf mediawiki-1.30.0.tar.gz
sudo cp -r mediawiki-1.30.0/* /var/www/html
sudo rm /var/www/html/index.html
sudo mysql -u root -p

# CREATE DATABASE mediawiki;
# CREATE USER mediawikiuser IDENTIFIED BY '1234';
# GRANT ALL PRIVILEGES ON mediawiki.* TO mediawikiuser IDENTIFIED BY '1234';
# exit;

# now open http://127.0.0.1

