var urlList = {};
var currentUI = false;

function closeRemoteUI() {
	if (currentUI) {
		currentUI = false;
		var root = document.getElementById('remote_ui');
		if (root.firstChild) {
			root.removeChild(root.firstChild);
		}
	}
}

function openRemoteUI(id) {
	closeRemoteUI();

	var root = document.getElementById('remote_ui');
	var iframe = document.createElement('iframe');
	iframe.src = urlList[id];
	root.appendChild(iframe);
	currentUI = id;

	document.getElementById('remote_control').style.display = 'block';
}

var deiveItemTag = ''
+'<div class="device">'
+'<img alt="-">'
+'</div>';

function getDeviceListResult() {
	if (this.readyState !== this.DONE) {
		return;
	}

	if (this.status !== 200) {
		slert('[ERROR][' + this.status + '] ' + 'getDeviceListResult');
	}

	console.log(typeof(this.response), this.response);
	
	var json = JSON.parse(this.responseText);
	for (var i = 0; i < json.length; ++i) {
		var item = json[i];
		var button = document.createElement('button');
		button.id = item.id;
		// location.host
		// location.hostname
		urlList[item.id] = location.origin + ':' + item.port;
		button.onclick = function () {
			openRemoteUI(this.id);
		};
		button.textContent = '[' + item.id + '] ' + item.nickname + ' (' + item.device + ')';
		button.disabled = item.disabled;

		var li = document.createElement('li');
		li.appendChild(button);
		document.getElementById('device_list').appendChild(li);
	}
}

window.onload = function () {
	var xhr = new XMLHttpRequest();
	xhr.responseType = 'json';
	xhr.onreadystatechange = getDeviceListResult;
	xhr.open('GET', '/do/get/device/list', true);
	xhr.send();	
};



document.getElementById('device_insert').onclick = function () {
	var txt = document.getElementById('device_id').value;
	if (txt.length > 0) {
		var xhr = new XMLHttpRequest();
		xhr.open('GET', '/do/device/insert', false);
		xhr.send('id="' + txt + '"');
		if (xhr.readyState === xhr.DONE) {
			if (xhr.status === 200) {

			} else {

			}
		}
	}
};

document.getElementById('device_remove').onclick = function () {
	var txt = document.getElementById('device_id').value;
	if (txt.length > 0) {
		var xhr = new XMLHttpRequest();
		xhr.open('POST', '/do/device/remove', false);
		xhr.send('id="' + txt + '"');
		if (xhr.readyState === xhr.DONE) {
			if (xhr.status === 200) {

			} else {
				
			}
		}
	}	
};

document.getElementById('remote_ui_close').onclick = function () {
	document.getElementById('remote_control').style.display = 'none';

	var remote_ui = document.getElementById('remote_ui');
	var iframe = document.querySelector('iframe');
	remote_ui.removeChild(iframe);
};



var begin = new Date();
var ws = new WebSocket('ws://' + window.location.host);
ws.onopen = function (event) { 
	console.log('onopen', event);

	
	
	return;
	setInterval(function () {
		//ws.send('Hello, world!');
		var end = new Date();
		var sec = (end-begin) / 1000.0;
		console.log('timeout', '['+sec+' sec]');
	}, 30000);
};
ws.onmessage = function (event) { 
	var end = new Date();
	var sec = (end-begin) / 1000.0;
	console.log('onmessage', '['+sec+' sec]', event);
	document.getElementById('socket_result').textContent = 'onmessage: ' + event.data + ' ['+sec+' sec]';
	
	var json = JSON.parse(event.data);
	if (json.event === "ServiceAdded") {
		document.getElementById(json.id).disabled = false;
		return;
	}
	if (json.event === "ServiceRemoved") {
		document.getElementById(json.id).disabled = true;
		return;
	}
	if (json.event === "Disconnected") {
		alert('Smart HUB disconnected');
		return;
	}
};
ws.onclose = function (event) { 
	console.log('onclose', event);
	var end = new Date();
	console.log((end-begin) / 1000.0 + ' sec');
};
ws.onerror = function (event) { 
	console.log('onerror', event);
};

document.getElementById('socket_send').onclick = function () {
	ws.send(document.getElementById('socket_text').value);
}