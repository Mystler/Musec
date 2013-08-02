/*==LICENSE==
This file is part of Musec.
Copyright (C) 2013 Florian Meiﬂner

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
==LICENSE==*/

\connect musec

SET client_encoding = 'UTF8';
SET standard_conforming_strings = off;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET escape_string_warning = off;

CREATE OR REPLACE LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION addScore(
    inUser varchar(64),
    inAverage decimal,
    inScore integer,
    inPlayed integer,
    inBingo integer,
    inStreak integer,
    inDifficulty varchar(32)
)
RETURNS void AS $$
DECLARE
    existingEntry integer;
BEGIN
    SELECT id INTO existingEntry FROM scores
    WHERE lower(username) = lower(inUser);
    IF NOT FOUND THEN
        INSERT INTO scores(username, average, score, played, bingo, streak, difficulty)
        VALUES(inUser, inAverage, inScore, inPlayed, inBingo, inStreak, inDifficulty);
    ELSE
        UPDATE scores SET scoredate = now(),
        average = inAverage,
        score = inScore,
        played = inPlayed,
        bingo = inBingo,
        streak = inStreak,
        difficulty = inDifficulty
        WHERE id = existingEntry;
    END IF;
END;
$$ LANGUAGE plpgsql;
ALTER FUNCTION addScore(varchar(64), decimal, integer, integer, integer, integer,
        varchar(64)) OWNER TO musec;
