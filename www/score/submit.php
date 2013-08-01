<?php
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
