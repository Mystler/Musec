SET client_encoding = 'UTF8';
SET standard_conforming_strings = off;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET escape_string_warning = off;

CREATE TABLE scores (
    id serial,
    user varchar(64),
    date timestamp DEFAULT NOW,
    average decimal
);
