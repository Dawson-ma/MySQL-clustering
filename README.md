# MySQL Clustering
This repository implemented [MySQL Loadable Function](https://dev.mysql.com/doc/extending-mysql/8.0/en/adding-loadable-function.html) for clustering algorithm including kmeans, hierarcial clustering, and DB scan, which allows these clustering methods can be used right in the MySQL.

If you find it useful, please give us a star or fork it:

[![GitHub stars](https://img.shields.io/github/stars/curzon01/MySQL-clustering.svg?style=social&label=Star)](https://github.com/Dawson-ma/MySQL-clustering/stargazers) [![GitHub forks](https://img.shields.io/github/forks/curzon01/MySQL-clustering.svg?style=social&label=Fork)](https://github.com/Dawson-ma/MySQL-clustering/network)

## Usage
Run following SQL queries to active the loadable functions:
```
CREATE FUNCTION Hierar_cluster RETURNS STRING
  SONAME 'clustering.dll';
```

## Installation in windows
Ensure the [CMake](http://www.cmake.org)(v2.6 or above) build utility is installed.  
Build a directory to store output files:
```C
mkdir build
cd build
```
Run the commands with your **generator** (ex. visual studio 16 2019) in the base directory:
```
cmake -G "generator" ..
devenv clustering_hierar.sln /build Release
```
This will build the MySQL library file **clustering.dll** in the directory **Release**.
Please copy **clustering.dll** to the [plugin_dir](https://dev.mysql.com/doc/refman/8.0/en/server-system-variables.html#sysvar_plugin_dir) directory.
If you can not find the plugin_dir, you can get the location of plugin_dir by running the following SQL queries:
```
select @@plugin_dir
```
