<?php
$dbhost = 'localhost:3306';
$dbuser = 'z';
$dbpass = 'wosiguwozai';

$conn = mysqli_connect($dbhost, $dbuser, $dbpass);
if(! $conn ) {
  die('connect failed: ' . mysqli_error($conn));
}
echo 'connect ok <br/>';

mysqli_select_db($conn, 'test');

$sql = 'select name from person';
$retval = mysqli_query($conn, $sql);
if(! $retval ) {
    die('can not find the database: ' . mysqli_error($conn));
}

while ($row = mysqli_fetch_array($retval, MYSQLI_ASSOC)) {
	echo $row['name'];
}

mysqli_close($conn);
?>
