create database final_project;
use final_project;

CREATE AGGREGATE FUNCTION Hierar_cluster RETURNS STRING
  SONAME 'clustering_hierar.dll';

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
    Hierar_cluster(3, "", age, weight)
FROM
    student;
    
drop function Hierar_cluster;
drop database final_project;

select @@plugin_dir

