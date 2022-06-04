create database example;
use example;

CREATE AGGREGATE FUNCTION Hierar_cluster RETURNS STRING
  SONAME 'clustering.dll';
CREATE AGGREGATE FUNCTION KMEANS RETURNS STRING
  SONAME 'clustering.dll';
CREATE AGGREGATE FUNCTION DBSCAN RETURNS STRING
  SONAME 'clustering.dll';
SET SQL_SAFE_UPDATES = 0;

CREATE TABLE iris (
    id INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
    `Sepal.Length` DOUBLE NOT NULL,
    `Sepal.Width` DOUBLE NOT NULL,
    `Petal.Length` DOUBLE NOT NULL,
    `Petal.Width` DOUBLE NOT NULL,
    `Species` VARCHAR(100) NOT NULL
);

INSERT INTO 
	iris(`Sepal.Length`, `Sepal.Width`, `Petal.Length`, `Petal.Width`, `Species`)
VALUES
	(5.1,3.5,1.4,.2,"Setosa"),
	(4.9,3,1.4,.2,"Setosa"),
	(4.7,3.2,1.3,.2,"Setosa"),
	(4.6,3.1,1.5,.2,"Setosa"),
	(5,3.6,1.4,.2,"Setosa"),
	(5.4,3.9,1.7,.4,"Setosa"),
	(4.6,3.4,1.4,.3,"Setosa"),
	(5,3.4,1.5,.2,"Setosa"),
	(4.4,2.9,1.4,.2,"Setosa"),
	(4.9,3.1,1.5,.1,"Setosa"),
	(5.4,3.7,1.5,.2,"Setosa"),
	(4.8,3.4,1.6,.2,"Setosa"),
	(4.8,3,1.4,.1,"Setosa"),
	(4.3,3,1.1,.1,"Setosa"),
	(5.8,4,1.2,.2,"Setosa"),
	(5.7,4.4,1.5,.4,"Setosa"),
	(5.4,3.9,1.3,.4,"Setosa"),
	(5.1,3.5,1.4,.3,"Setosa"),
	(5.7,3.8,1.7,.3,"Setosa"),
	(5.1,3.8,1.5,.3,"Setosa"),
	(5.4,3.4,1.7,.2,"Setosa"),
	(5.1,3.7,1.5,.4,"Setosa"),
	(4.6,3.6,1,.2,"Setosa"),
	(5.1,3.3,1.7,.5,"Setosa"),
	(4.8,3.4,1.9,.2,"Setosa"),
	(5,3,1.6,.2,"Setosa"),
	(5,3.4,1.6,.4,"Setosa"),
	(5.2,3.5,1.5,.2,"Setosa"),
	(5.2,3.4,1.4,.2,"Setosa"),
	(4.7,3.2,1.6,.2,"Setosa"),
	(4.8,3.1,1.6,.2,"Setosa"),
	(5.4,3.4,1.5,.4,"Setosa"),
	(5.2,4.1,1.5,.1,"Setosa"),
	(5.5,4.2,1.4,.2,"Setosa"),
	(4.9,3.1,1.5,.2,"Setosa"),
	(5,3.2,1.2,.2,"Setosa"),
	(5.5,3.5,1.3,.2,"Setosa"),
	(4.9,3.6,1.4,.1,"Setosa"),
	(4.4,3,1.3,.2,"Setosa"),
	(5.1,3.4,1.5,.2,"Setosa"),
	(5,3.5,1.3,.3,"Setosa"),
	(4.5,2.3,1.3,.3,"Setosa"),
	(4.4,3.2,1.3,.2,"Setosa"),
	(5,3.5,1.6,.6,"Setosa"),
	(5.1,3.8,1.9,.4,"Setosa"),
	(4.8,3,1.4,.3,"Setosa"),
	(5.1,3.8,1.6,.2,"Setosa"),
	(4.6,3.2,1.4,.2,"Setosa"),
	(5.3,3.7,1.5,.2,"Setosa"),
	(5,3.3,1.4,.2,"Setosa"),
	(7,3.2,4.7,1.4,"Versicolor"),
	(6.4,3.2,4.5,1.5,"Versicolor"),
	(6.9,3.1,4.9,1.5,"Versicolor"),
	(5.5,2.3,4,1.3,"Versicolor"),
	(6.5,2.8,4.6,1.5,"Versicolor"),
	(5.7,2.8,4.5,1.3,"Versicolor"),
	(6.3,3.3,4.7,1.6,"Versicolor"),
	(4.9,2.4,3.3,1,"Versicolor"),
	(6.6,2.9,4.6,1.3,"Versicolor"),
	(5.2,2.7,3.9,1.4,"Versicolor"),
	(5,2,3.5,1,"Versicolor"),
	(5.9,3,4.2,1.5,"Versicolor"),
	(6,2.2,4,1,"Versicolor"),
	(6.1,2.9,4.7,1.4,"Versicolor"),
	(5.6,2.9,3.6,1.3,"Versicolor"),
	(6.7,3.1,4.4,1.4,"Versicolor"),
	(5.6,3,4.5,1.5,"Versicolor"),
	(5.8,2.7,4.1,1,"Versicolor"),
	(6.2,2.2,4.5,1.5,"Versicolor"),
	(5.6,2.5,3.9,1.1,"Versicolor"),
	(5.9,3.2,4.8,1.8,"Versicolor"),
	(6.1,2.8,4,1.3,"Versicolor"),
	(6.3,2.5,4.9,1.5,"Versicolor"),
	(6.1,2.8,4.7,1.2,"Versicolor"),
	(6.4,2.9,4.3,1.3,"Versicolor"),
	(6.6,3,4.4,1.4,"Versicolor"),
	(6.8,2.8,4.8,1.4,"Versicolor"),
	(6.7,3,5,1.7,"Versicolor"),
	(6,2.9,4.5,1.5,"Versicolor"),
	(5.7,2.6,3.5,1,"Versicolor"),
	(5.5,2.4,3.8,1.1,"Versicolor"),
	(5.5,2.4,3.7,1,"Versicolor"),
	(5.8,2.7,3.9,1.2,"Versicolor"),
	(6,2.7,5.1,1.6,"Versicolor"),
	(5.4,3,4.5,1.5,"Versicolor"),
	(6,3.4,4.5,1.6,"Versicolor"),
	(6.7,3.1,4.7,1.5,"Versicolor"),
	(6.3,2.3,4.4,1.3,"Versicolor"),
	(5.6,3,4.1,1.3,"Versicolor"),
	(5.5,2.5,4,1.3,"Versicolor"),
	(5.5,2.6,4.4,1.2,"Versicolor"),
	(6.1,3,4.6,1.4,"Versicolor"),
	(5.8,2.6,4,1.2,"Versicolor"),
	(5,2.3,3.3,1,"Versicolor"),
	(5.6,2.7,4.2,1.3,"Versicolor"),
	(5.7,3,4.2,1.2,"Versicolor"),
	(5.7,2.9,4.2,1.3,"Versicolor"),
	(6.2,2.9,4.3,1.3,"Versicolor"),
	(5.1,2.5,3,1.1,"Versicolor"),
	(5.7,2.8,4.1,1.3,"Versicolor"),
	(6.3,3.3,6,2.5,"Virginica"),
	(5.8,2.7,5.1,1.9,"Virginica"),
	(7.1,3,5.9,2.1,"Virginica"),
	(6.3,2.9,5.6,1.8,"Virginica"),
	(6.5,3,5.8,2.2,"Virginica"),
	(7.6,3,6.6,2.1,"Virginica"),
	(4.9,2.5,4.5,1.7,"Virginica"),
	(7.3,2.9,6.3,1.8,"Virginica"),
	(6.7,2.5,5.8,1.8,"Virginica"),
	(7.2,3.6,6.1,2.5,"Virginica"),
	(6.5,3.2,5.1,2,"Virginica"),
	(6.4,2.7,5.3,1.9,"Virginica"),
	(6.8,3,5.5,2.1,"Virginica"),
	(5.7,2.5,5,2,"Virginica"),
	(5.8,2.8,5.1,2.4,"Virginica"),
	(6.4,3.2,5.3,2.3,"Virginica"),
	(6.5,3,5.5,1.8,"Virginica"),
	(7.7,3.8,6.7,2.2,"Virginica"),
	(7.7,2.6,6.9,2.3,"Virginica"),
	(6,2.2,5,1.5,"Virginica"),
	(6.9,3.2,5.7,2.3,"Virginica"),
	(5.6,2.8,4.9,2,"Virginica"),
	(7.7,2.8,6.7,2,"Virginica"),
	(6.3,2.7,4.9,1.8,"Virginica"),
	(6.7,3.3,5.7,2.1,"Virginica"),
	(7.2,3.2,6,1.8,"Virginica"),
	(6.2,2.8,4.8,1.8,"Virginica"),
	(6.1,3,4.9,1.8,"Virginica"),
	(6.4,2.8,5.6,2.1,"Virginica"),
	(7.2,3,5.8,1.6,"Virginica"),
	(7.4,2.8,6.1,1.9,"Virginica"),
	(7.9,3.8,6.4,2,"Virginica"),
	(6.4,2.8,5.6,2.2,"Virginica"),
	(6.3,2.8,5.1,1.5,"Virginica"),
	(6.1,2.6,5.6,1.4,"Virginica"),
	(7.7,3,6.1,2.3,"Virginica"),
	(6.3,3.4,5.6,2.4,"Virginica"),
	(6.4,3.1,5.5,1.8,"Virginica"),
	(6,3,4.8,1.8,"Virginica"),
	(6.9,3.1,5.4,2.1,"Virginica"),
	(6.7,3.1,5.6,2.4,"Virginica"),
	(6.9,3.1,5.1,2.3,"Virginica"),
	(5.8,2.7,5.1,1.9,"Virginica"),
	(6.8,3.2,5.9,2.3,"Virginica"),
	(6.7,3.3,5.7,2.5,"Virginica"),
	(6.7,3,5.2,2.3,"Virginica"),
	(6.3,2.5,5,1.9,"Virginica"),
	(6.5,3,5.2,2,"Virginica"),
	(6.2,3.4,5.4,2.3,"Virginica"),
	(5.9,3,5.1,1.8,"Virginica");

/* SPLIT THE STRING*/
DELIMITER $$
CREATE FUNCTION SPLIT_STR(
  x VARCHAR(10000),
  delim VARCHAR(10),
  pos INT
)
RETURNS VARCHAR(10000) DETERMINISTIC
BEGIN 
    RETURN REPLACE(SUBSTRING(SUBSTRING_INDEX(x, delim, pos),
       LENGTH(SUBSTRING_INDEX(x, delim, pos -1)) + 1),
       delim, '');
END$$
DELIMITER ;

DELIMITER #
CREATE PROCEDURE `cluster`(
	IN `STRING` VARCHAR(10000),
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

/* K-means */
/*
  KMEANS: aggregate function
  KMEANS(int k, int max_iter, double features...)
  Input arguments:
   k       = number of observables
   max_iter  = max iteration
*/
SELECT * FROM iris;
CALL cluster((SELECT KMEANS(3, 50, `Sepal.Length`, `Sepal.Width`, `Petal.Length`, `Petal.Width`)FROM iris), 'iris');
SELECT * FROM iris WHERE cluster=0;
SELECT * FROM iris WHERE cluster=1;
SELECT * FROM iris WHERE cluster=2;
ALTER TABLE iris drop cluster;

/* Hierarchial clustering */
/*
  Hierar_cluster: aggregate function
  Hierar_cluster(int n, string method, double features...)
  Input arguments:
   n       = number of observables
   method  = cluster metric: "single", "complete", "average", "median"
*/
SELECT * FROM iris;
CALL cluster((SELECT Hierar_cluster(3, "", `Sepal.Length`, `Sepal.Width`, `Petal.Length`, `Petal.Width`)FROM iris), 'iris');
SELECT * FROM iris WHERE cluster=0;
SELECT * FROM iris WHERE cluster=1;
SELECT * FROM iris WHERE cluster=2;
CALL cluster((SELECT Hierar_cluster(5, "average", `Sepal.Length`, `Sepal.Width`, `Petal.Length`, `Petal.Width`)FROM iris), 'iris');
SELECT * FROM iris WHERE cluster=0;
SELECT * FROM iris WHERE cluster=1;
SELECT * FROM iris WHERE cluster=2;
SELECT * FROM iris WHERE cluster=4;
SELECT * FROM iris WHERE cluster=5;
ALTER TABLE iris drop cluster;

/* DBSCAN aggregate */
/* 
   DBSCAN(double epsilon, int minPoint, double features ...)
   Input arguments:
    epsilon   = radius of query region
    minPoint  = minima num. of the points in a query region
   outputs:
    -1: outlier
    others: groups
*/
CALL cluster((SELECT DBSCAN(0.2, 2, `Sepal.Length`, `Sepal.Width`, `Petal.Length`, `Petal.Width`)FROM iris), 'iris');
SELECT * FROM iris;
SELECT * FROM iris WHERE cluster=0;
SELECT * FROM iris WHERE cluster=1;
SELECT * FROM iris WHERE cluster=2;
SELECT * FROM iris WHERE cluster=-1;
ALTER TABLE iris drop cluster;



DROP FUNCTION Hierar_cluster;
DROP FUNCTION KMEANS;
DROP FUNCTION DBSCAN;
DROP DATABASE example;


