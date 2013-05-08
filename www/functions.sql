SET client_encoding = 'UTF8';
SET standard_conforming_strings = off;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET escape_string_warning = off;

CREATE OR REPLACE LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION addScore(
    inUser varchar(64),
    inAverage decimal
)
RETURNS void AS $$
DECLARE
    existingEntry integer
BEGIN
    SELECT id INTO existingEntry FROM scores
    WHERE user = inUser
    IF NOT FOUND THEN
        INSERT INTO scores(user, average)
        VALUES(inUser, inAverage);
    ELSE
        UPDATE SET average = inAverage
        WHERE user = inUser;
    END IF;
END;
$$ LANGUAGE plpgsql;
