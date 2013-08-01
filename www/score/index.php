<html>
<head><title>Musec Score</title>
<body>
<?php
    require_once "config.ini.php";

    $dbconn = pg_connect("host={$dbhost} dbname={$dbname} user={$dbuser} password={$dbpass}");
    if (!$dbconn) {
        echo "Database connection error!";
        exit;
    }

    $result = pg_query($dbconn, "SELECT username, average, scoredate FROM scores");
    if (!$result) {
        echo "Error on fetching data!";
        pg_close($dbconn);
        exit;
    }

    while ($row = pg_fetch_row($result)) {
        echo "{$row[0]} {$row[1]} {$row[2]}<br>";
    }
    pg_free_result($result);

    pg_close($dbconn);
?>
</body>
