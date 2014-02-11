Musec
=====

Do you know a song by just listening to one second of it?

Choose folders and files of your own music library and see if you can
guess title, artist and album by only hearing less than five seconds
from somewhere in the song. Right answers will add points to your score.


Dependencies
------------

This program has the following requirements:
- C++11 compiler (GCC 4.6+, Clang 3.0+, MSVC 11+)
- CMake 2.8.11.2 or newer
- Qt 5.2.1 Framework

For running a score webservice, you will also need:
- A webserver (e.g. Apache2 or nginx) that is configured correctly for
  PHP5
- PostgreSQL Server (tested with version 9.1)


Building
--------

I recommend to set the following CMake variables:
- *CMAKE_INSTALL_PREFIX* - Path for the install target
- *CMAKE_PREFIX_PATH* - Qt5 directory


Webservice
----------

First you have to set up the database. You have to create a user and
a database called *musec*. Assuming that you already have installed
a **PostgreSQL** server, you can do something like the following on Linux:

```
$ sudo -u postgres psql
postgres=# CREATE USER musec WITH PASSWORD '<password>';
postgres=# CREATE DATABASE musec WITH OWNER = musec ENCODING = 'UTF8';
postgres=# \q
```

Now you can run the scripts to initialize the database.
```
sudo -u postgres psql -d musec < www/init.sql
sudo -u postgres psql -d musec < www/functions.sql
```

You can only run the init script once, but you can run the functions
script again, whenever it has been updated.

The files for the webserver are located in the folder *www/score*.
After copying or linking the files, you have to make sure the config
file matches your database configuration.

```
cp config-sample.ini.php config.ini.php
nano config.ini.php
```

If you are using **Apache2**, the *.htaccess* file in the folder should
should take care of the necessary rewrite rules.
However, if you are using **nginx**, you have to add your own rewrite
rule to the server or location configuration.

```
rewrite ^/musec/score/u/(.*)$ /musec/score/index.php?user=$1;
```
