<html>
<head><title>Musec Score</title>
<style>
body {
    text-align: center;
}
th, td {
    padding: 1px 6px;
}
.left {
    text-align: left;
}
.center {
    text-align: center;
}
table.center {
    margin-left:auto;
    margin-right:auto;
}
</style>
<body>
<h1>Musec Scoreboard</h1>
Welcome to the Musec scoreboard.<br>
If you want to add your own score, you can do so by clicking <i>Info->Submit Score</i>
in the client. Entries for existing users will be overwritten.<br><br>
<table class="center" border="1">
    <tr>
        <th class="left">User</th>
        <th>Average</th>
        <th>Score</th>
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

    $result = pg_query($dbconn, "SELECT username, average, score, played, bingo, streak,
            difficulty, to_char(scoredate, 'DD Mon YYYY, HH12:MI AM') FROM scores
            ORDER BY average DESC, score ASC, streak ASC, bingo ASC, played ASC");
    if (!$result) {
        echo "Error on fetching data!";
        pg_close($dbconn);
        exit;
    }

    while ($row = pg_fetch_row($result)) {
?>
    <tr>
        <td><?php echo $row[0]; ?></td>
        <td class="center"><?php echo $row[1]; ?></td>
        <td class="center"><?php echo $row[2]; ?></td>
        <td class="center"><?php echo $row[3]; ?></td>
        <td class="center"><?php echo $row[4]; ?></td>
        <td class="center"><?php echo $row[5]; ?></td>
        <td class="center"><?php echo $row[6]; ?></td>
        <td><?php echo $row[7]; ?></td>
    </tr>
<?php
    }
    pg_free_result($result);

    pg_close($dbconn);
?>
</table>
</body>
