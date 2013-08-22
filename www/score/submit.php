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

$muser = $_POST["user"];
$mavg = $_POST["avg"];
$mscore = $_POST["score"];
$mplayed = $_POST["played"];
$mbingo = $_POST["bingo"];
$mstreak = $_POST["streak"];
$mdiff = $_POST["diff"];

if (!isset($muser) || !isset($mavg) || !isset($mscore) || !isset($mplayed) ||
        !isset($mbingo) || !isset($mstreak) || !isset($mdiff))
    die("Invalid request!");

require_once "config.ini.php";

$dbconn = pg_connect("host={$dbhost} dbname={$dbname} user={$dbuser} password={$dbpass}");
if (!$dbconn)
    die("Database connection error!");

$result = pg_query($dbconn, "SELECT addscore('{$muser}', {$mavg}, {$mscore}, {$mplayed},
                   {$mbingo}, {$mstreak}, '{$mdiff}')");
if (!$result) {
    pg_close($dbconn);
    die("Error on submitting data!");
}

pg_close($dbconn);
?>
