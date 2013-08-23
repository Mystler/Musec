<?php
/*==LICENSE==
This file is part of Musec.
Copyright (C) 2013 Florian Meißner

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
?>
<!DOCTYPE html>
<html>
<head>
    <title>Musec Score</title>
    <link href="scoreboard.css" rel="stylesheet" type="text/css">
    <link href='http://fonts.googleapis.com/css?family=Droid+Sans:400,700' rel='stylesheet' type='text/css'>
    <meta charset="UTF-8">
</head>
<body>
    <h1>Musec Scoreboard</h1>
    Welcome to the Musec scoreboard.<br>
    If you want to add your own score, you can do so by clicking <i>Info->Submit Score</i>
    in the client. Entries for existing users will be overwritten.<br><br>
    <table>
        <tr>
            <th class="left">User</th>
            <th>Score</th>
            <th>Average</th>
            <th>Played</th>
            <th>Bingo!</th>
            <th>Streak</th>
            <th>Difficulty</th>
            <th class="left">Date</th>
        </tr>
<?php
require_once "config.ini.php";

$dbconn = pg_connect("host={$dbhost} dbname={$dbname} user={$dbuser} password={$dbpass}");
if (!$dbconn) {
    echo "Database connection error!";
    exit;
}

$result = pg_query($dbconn, "SELECT username, score, average, played, bingo, streak,
        difficulty, to_char(scoredate, 'DD Mon YYYY, HH12:MI AM') FROM scores
        ORDER BY score DESC, average DESC, streak DESC, bingo DESC, played DESC");
if (!$result) {
    echo "Error on fetching data!";
    pg_close($dbconn);
    exit;
}

while ($row = pg_fetch_row($result)) {
?>
        <tr>
            <td class="left"><?php echo $row[0]; ?></td>
            <td><?php echo $row[1];?></td>
            <td><?php echo $row[2];?></td>
            <td><?php echo $row[3];?></td>
            <td><?php echo $row[4];?></td>
            <td><?php echo $row[5];?></td>
            <td><?php echo $row[6];?></td>
            <td class="left"><?php echo $row[7]; ?></td>
        </tr>
<?php
}
pg_free_result($result);

pg_close($dbconn);
?>
    </table>
</body>
