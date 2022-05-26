create database final_project;
use final_project;

CREATE AGGREGATE FUNCTION KMEANS RETURNS STRING
  SONAME 'clustering.dll';

CREATE TABLE student (
    ID INT NOT NULL AUTO_INCREMENT,
    age double,
    weight double,
    PRIMARY KEY (ID)
);
INSERT INTO student (age, weight) VALUES (1, 6);
INSERT INTO student (age, weight) VALUES (2, 7);
INSERT INTO student (age, weight) VALUES (3, 8);
INSERT INTO student (age, weight) VALUES (4, 9);
INSERT INTO student (age, weight) VALUES (5, 10);

SELECT 
    KMEANS(3, 5, age, weight)
FROM
    student;

drop function KMEANS;
drop database final_project;