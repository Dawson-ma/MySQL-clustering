create database final_project;
use final_project;

CREATE AGGREGATE FUNCTION Hierar_cluster RETURNS STRING
  SONAME 'clustering.dll';
CREATE AGGREGATE FUNCTION KMEANS RETURNS STRING
  SONAME 'clustering.dll';
CREATE TABLE iris_data (
    id INT NOT NULL PRIMARY KEY,
    `Sepal.Length` FLOAT NOT NULL,
    `Sepal.Width` FLOAT NOT NULL,
    `Petal.Length` FLOAT NOT NULL,
    `Petal.Width` FLOAT NOT NULL,
    `Species` VARCHAR(100) NOT NULL,
    `SpeciesId` INT NOT NULL
);

CREATE TABLE student (
    ID INT NOT NULL AUTO_INCREMENT,
    age double,
    weight double,
    PRIMARY KEY (ID)
);
INSERT INTO student (age, weight) VALUES (1, 1);
INSERT INTO student (age, weight) VALUES (1, 2);
INSERT INTO student (age, weight) VALUES (1, 3);
INSERT INTO student (age, weight) VALUES (10, 9);
INSERT INTO student (age, weight) VALUES (10, 10);

SELECT 
    KMEANS(3, 1000, age, weight)
FROM
    student;
    
SELECT 
    Hierar_cluster(3, "", age, weight)
FROM
    student;
    
drop function Hierar_cluster;
drop function KMEANS;
drop database final_project;

select @@plugin_dir

