<?php
/*==LICENSE==
This file is part of Musec.
Copyright (C) 2013 Florian Meißner

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
==LICENSE==*/

require_once "config.ini.php";

function get($user) {
    global $dbconn;
    $result = pg_query_params($dbconn, "SELECT score FROM scores WHERE lower(username) = lower($1)", array($user));
    if (($score = pg_fetch_result($result, 0)) !== false) {
        echo $score;
        pg_free_result($result);
    } else {
        header("HTTP/1.1 404 Not Found");
    }
}

function post() {
    global $dbconn;
    $muser = $_POST["user"];
    $mavg = $_POST["avg"];
    $mscore = $_POST["score"];
    $mplayed = $_POST["played"];
    $mbingo = $_POST["bingo"];
    $mstreak = $_POST["streak"];
    $mdiff = $_POST["diff"];

    if (!isset($muser) || !isset($mavg) || !isset($mscore) || !isset($mplayed) ||
            !isset($mbingo) || !isset($mstreak) || !isset($mdiff)) {
        header("HTTP/1.1 400 Bad Request");
        return;
    }

    $result = pg_query_params($dbconn, "SELECT addscore($1, $2, $3, $4, $5, $6, $7)", array($muser, $mavg, $mscore,
            $mplayed, $mbingo, $mstreak, $mdiff));
    if (!$result) {
        header("HTTP/1.1 409 Conflict");
    } else {
        header("HTTP/1.1 200 OK");
        pg_free_result($result);
    }
}

function scoreboard() {
    global $dbconn;
?>
<!DOCTYPE html>
<html>
<head>
    <title>Musec Score</title>
    <link href="scoreboard.css" rel="stylesheet" type="text/css">
    <link href='http://fonts.googleapis.com/css?family=Droid+Sans:400,700' rel='stylesheet' type='text/css'>
    <meta http-equiv="X-UA-Compatible" content="IE=Edge"/>
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
    $result = pg_query($dbconn, "SELECT username, score, average, played, bingo, streak,
            difficulty, to_char(scoredate, 'DD Mon YYYY, HH12:MI AM') FROM scores
            ORDER BY score DESC, average DESC, streak DESC, bingo DESC, played DESC");
    if (!$result) {
        header("HTTP/1.1 404 Not Found");
        return;
    } else {
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
            </tbody>
<?php
        }
        pg_free_result($result);
    }

    $result = pg_query($dbconn, "SELECT SUM(score), round(SUM(score) / SUM(played)::decimal, 2),
            SUM(played), SUM(bingo), SUM(streak) FROM scores");
    if ($row = pg_fetch_row($result)) {
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
    } else {
        header("HTTP/1.1 404 Not Found");
        return;
    }
?>
        </table>
    </div>
</body>
<?php
}

$dbconn = pg_connect("host={$dbhost} dbname={$dbname} user={$dbuser} password={$dbpass}");
if (!$dbconn) {
    header("HTTP/1.1 503 Service Unavailable");
    exit;
}

header("Content-Type: text/html; charset=UTF-8");

switch($_SERVER["REQUEST_METHOD"]) {
case "GET":
    if (isset($_GET["user"]))
        get($_GET["user"]);
    else
        scoreboard();
    break;
case "POST":
    post();
    break;
default:
    header("HTTP/1.1 405 Method Not Allowed");
}

pg_close($dbconn);
?>
