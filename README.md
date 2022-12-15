# MySQL Clustering
This repository implemented [MySQL Loadable Function](https://dev.mysql.com/doc/extending-mysql/8.0/en/adding-loadable-function.html) for clustering algorithm including kmeans, hierarchical clustering, and DB scan, which allows these clustering methods can be used right in the MySQL.

If you find it useful, please give us a star or fork it:

[![GitHub stars](https://img.shields.io/github/stars/curzon01/MySQL-clustering.svg?style=social&label=Star)](https://github.com/Dawson-ma/MySQL-clustering/stargazers) [![GitHub forks](https://img.shields.io/github/forks/curzon01/MySQL-clustering.svg?style=social&label=Fork)](https://github.com/Dawson-ma/MySQL-clustering/network)

## Function
```
KMEANS(int k, int max_iter, double features...)
  Input arguments:
   k       = number of observables
   max_iter  = max iteration
```
```
Hierar_cluster(int n, string method, double features...)
   Input arguments:
   n       = number of observables
   method  = cluster metric: "single", "complete", "average", "median"
```
```
DBSCAN(double epsilon, int minPoint, double features ...)
   Input arguments:
    epsilon   = radius of query region
    minPoint  = minima num. of the points in a query region
```

## Usage
### Create
Run following SQL queries to active the loadable functions:
```SQL
CREATE AGGREGATE FUNCTION KMEANS RETURNS STRING
  SONAME 'clustering.dll';
CREATE AGGREGATE FUNCTION DBSCAN RETURNS STRING
  SONAME 'clustering1.dll';
CREATE AGGREGATE FUNCTION Hierar_cluster RETURNS STRING
  SONAME 'clustering.dll';
```
Create SQL procedure functions to combine the results with selected table:
```SQL
DELIMITER $$
CREATE FUNCTION SPLIT_STR(
  x LONGTEXT,
  delim VARCHAR(10),
  pos INT
)
RETURNS LONGTEXT DETERMINISTIC
BEGIN 
    RETURN REPLACE(SUBSTRING(SUBSTRING_INDEX(x, delim, pos),
       LENGTH(SUBSTRING_INDEX(x, delim, pos -1)) + 1),
       delim, '');
END$$
DELIMITER ;

DELIMITER #
CREATE PROCEDURE `cluster`(
	IN `STRING` LONGTEXT,
    IN `T` VARCHAR(1000))
BEGIN
DECLARE v_max INT UNSIGNED DEFAULT 1;
DECLARE v_counter INT UNSIGNED DEFAULT 0;
SET SQL_SAFE_UPDATES = 0;
SET @v_counter = 1;
SET @TABLE1 = `T`;
SET @VAR = NULL;
SET @C = 0;
SET @s = CONCAT('SELECT COUNT(`ID`) INTO @C FROM ', @TABLE1); 
SET @R = (SELECT table_name FROM information_schema.tables WHERE table_name=`T`);
SELECT @R;
PREPARE stmt1 FROM @s; 
EXECUTE stmt1;
SET v_max = @C +1;
DEALLOCATE PREPARE stmt1;
  START TRANSACTION;
  IF NOT EXISTS(SELECT NULL
            FROM INFORMATION_SCHEMA.COLUMNS
           WHERE table_name = @TABLE1 AND
           column_name = 'cluster') THEN
	SET @d = CONCAT('ALTER TABLE ', @TABLE1, ' ADD COLUMN cluster INT');
    PREPARE stmt2 FROM @d; 
	EXECUTE stmt2;
    DEALLOCATE PREPARE stmt2;
  END IF;
  WHILE @v_counter < v_max DO
	SET @VAR = (SELECT SPLIT_STR(`STRING`, ',', @v_counter));
    /*SELECT @VAR := (SELECT SPLIT_STR(`STRING`, ',', @v_counter));*/
    SET @e = CONCAT('UPDATE ', @TABLE1, ' SET cluster = @VAR WHERE ID = @v_counter');
    PREPARE stmt3 FROM @e; 
	EXECUTE stmt3;
    SET @v_counter=@v_counter+1;
    DEALLOCATE PREPARE stmt3;
  END WHILE;
SET @f = CONCAT('SELECT * FROM ', @TABLE1);
    PREPARE stmt4 FROM @f; 
	EXECUTE stmt4;
DEALLOCATE PREPARE stmt4;
END #
DELIMITER ;
```
Use functions to cluster data with selected features:
```SQL
CALL cluster((SELECT KMEANS(5, 20, features_1, features_2, features_3)FROM table), 'table');
CALL cluster((SELECT Hierar_cluster(5, 'simple', features_1, features_2, features_3)FROM table), 'table');
CALL cluster((SELECT DBScan(3, 'average', features_1, features_2, features_3)FROM table), 'table');
```

### Drop
Release loadable functions:
```SQL
DROP FUNCTION IF EXISTS KMEANS;
DROP FUNCTION IF EXISTS Hierar_cluster;
DROP FUNCTION IF EXISTS DBScan;
```
Release procedure functions:
```SQL
DROP FUNCTION SPLIT_STR;
DROP PROCEDURE cluster;
```

## Algorithm
### K-means
![](https://github.com/Dawson-ma/MySQL-clustering/blob/main/img/kmeans.png)

### Hierarchicial clustering
![](https://upload.wikimedia.org/wikipedia/commons/thumb/1/12/Iris_dendrogram.png/800px-Iris_dendrogram.png)  
source: https://cran.r-project.org/web/packages/dendextend/vignettes/Cluster_Analysis.html

### DBSCAN
![](https://github.com/Dawson-ma/MySQL-clustering/blob/main/img/DBSCAN.png)


## Installation
### 1. Using dll file
Please copy **clustering.dll** to the [plugin_dir](https://dev.mysql.com/doc/refman/8.0/en/server-system-variables.html#sysvar_plugin_dir) directory.
If you can not find the plugin_dir, you can get the location of plugin_dir by running the following SQL queries:
```SQL
select @@plugin_dir
```

### 2. cmake
If method 1 does not work, please follow the below steps to build dll file.
Ensure the [CMake](http://www.cmake.org)(v2.6 or above) build utility is installed.  
Build a directory to store output files:
```cmd
mkdir build
cd build
```
Run the commands with your **generator** (ex. visual studio 16 2019) in the base directory:
```cmd
cmake -G "generator" ..
devenv clustering.sln /build Release
```
This will build the MySQL library file **clustering.dll** in the directory **Release**.
Please copy **clustering.dll** to the [plugin_dir](https://dev.mysql.com/doc/refman/8.0/en/server-system-variables.html#sysvar_plugin_dir) directory.
If you can not find the plugin_dir, you can get the location of plugin_dir by running the following SQL queries:
```SQL
select @@plugin_dir
```
