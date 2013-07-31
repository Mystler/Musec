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
    existingEntry integer
BEGIN
    SELECT id INTO existingEntry FROM scores
    WHERE username = inUser
    IF NOT FOUND THEN
        INSERT INTO scores(username, average, score, played, bingo, streak, difficulty)
        VALUES(inUser, inAverage, inScore, inPlayed, inBingo, inStreak, inDifficulty);
    ELSE
        UPDATE SET average = inAverage,
        score = inScore,
        played = inPlayed,
        bingo = inBingo,
        streak = inStreak,
        difficulty = inDifficulty
        WHERE username = inUser;
    END IF;
END;
$$ LANGUAGE plpgsql;
