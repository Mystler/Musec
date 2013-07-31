SET client_encoding = 'UTF8';
SET standard_conforming_strings = off;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET escape_string_warning = off;

CREATE TABLE scores (
    id serial,
    username varchar(64) NOT NULL,
    scoredate timestamp DEFAULT now(),
    average decimal,
    score integer,
    played integer,
    bingo integer,
    streak integer,
    difficulty varchar(32)
);
