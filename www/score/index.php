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
    <div id="container">
        <h1>Musec Scoreboard</h1>
        Welcome to the Musec scoreboard.<br>
        If you want to add your own score, you can do so by clicking <i>Info->Submit Score</i>
        in the client. Entries for existing users will be overwritten.<br><br>
        <table>
            <tbody>
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
            </tbody>
<?php
require_once "config.ini.php";

$dbconn = pg_connect("host={$dbhost} dbname={$dbname} user={$dbuser} password={$dbpass}");
if (!$dbconn)
    die("Database connection error!");

$result = pg_query($dbconn, "SELECT username, score, average, played, bingo, streak,
        difficulty, to_char(scoredate, 'DD Mon YYYY, HH12:MI AM') FROM scores
        ORDER BY score DESC, average DESC, streak DESC, bingo DESC, played DESC");
if (!$result) {
    pg_close($dbconn);
    die("Error on fetching data!");
}
while ($row = pg_fetch_row($result)) {
?>
            <tbody class="userentry">
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
                <tr class="userdetails">
                    <td colspan="8">TODO</td>
                </tr>
            </tbody>
<?php
}
pg_free_result($result);

$result = pg_query($dbconn, "SELECT SUM(score), round(SUM(score) / SUM(played)::decimal, 2),
        SUM(played), SUM(bingo), SUM(streak) FROM scores");
if (!$result) {
    pg_close($dbconn);
    die("Error on fetching data!");
}
$row = pg_fetch_row($result, 0);
?>
            <tbody>
                <tr>
                    <td class="left"><b>Total</b></td>
                    <td><b><?php echo $row[0];?></b></td>
                    <td><b><?php echo $row[1];?></b></td>
                    <td><b><?php echo $row[2];?></b></td>
                    <td><b><?php echo $row[3];?></b></td>
                    <td><b><?php echo $row[4];?></b></td>
                    <td></td>
                    <td class="left"></td>
                </tr>
            </tbody>
<?php
pg_free_result($result);

pg_close($dbconn);
?>
        </table>
    </div>
</body>
