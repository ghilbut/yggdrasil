var rabbitHole = (function () {
	var rh_ = {
		request: request,
		notify: notify,
		onevent: false
	};

	var ws = false;

	function Request(query, data, onresponse, onerror) {
		var xhr = new XMLHttpRequest()
			, data = data
			, onresponse = (typeof(onresponse) === 'function' ? onresponse : false)
			, onerror = (typeof(onerror) === 'function' ? onerror : false);
		xhr.onreadystatechange = resultCallback;
		xhr.open('POST', '/do/' + query, false);
		xhr.send(data);

		function resultCallback() {
			if (this.readyState !== this.DONE) {
				return;
			}

			if (this.status === 200) {
				if (onresponse) {
					onresponse(this.response)
				}
			} else {
				// TODO(ghilbut): it is not designed which how to notify error, yet.
				if (onerror) {
					onerror();
				}
			}
		}
	}

	function request(query, data, onresponse, onerror) {
		// TODO(ghilbut):
		// redesign get parameters of query
		// key:value object is standard

		var req = false;

		if (typeof(query) !== 'string') {
			console.log('[ERROR] query should be string type.', query);
			return false;
		}

		if (typeof(data) !== 'string') {
			if (data.constructor !== Object) {
				console.log('[ERROR]', 'invalid data type', data);
				return false;
			}
			data = JSON.stringify(data);
		}

		req = new Request(query, data, onresponse, onerror);
		return true;
	}

	function notify(data) {
		if (typeof(data) !== 'string') {
			if (data.constructor !== Object) {
				console.log('[ERROR]', 'invalid data type', data);
				return false;
			}
			data = JSON.stringify(data);
		}
		if (ws && ws.readyState === ws.OPEN) {
			ws.send(data);
			return true;
		}
		return false;
	}

	var ws_onopen = function (event) { 
		console.log('ws_onopen', event);
	};
	var ws_onmessage = function (event) { 
		console.log('ws_onmessage', event);
		var cb = rh_.onevent;
		if (cb && typeof(cb) === 'function') {
			cb(event.data);
		}
	};
	var ws_onclose = function (event) { 
		console.log('ws_onclose', event);
		setTimeout(function () {
			ws = ws_open(window.location.host);		
		}, 1000);
	};
	var ws_onerror = function (event) { 
		console.log('ws_onerror', event);
	};

	function ws_open(host) {
		var conn = new WebSocket('ws://' + host);
		conn.onopen = ws_onopen;
		conn.onmessage = ws_onmessage;
		conn.onclose = ws_onclose;
		conn.onerror = ws_onerror;
		return conn;
	}

	ws = ws_open(window.location.host);

	setInterval(function () {
		if (ws && ws.readyState === ws.OPEN) {
			ws.send('');
		}
	}, 3000);

	return rh_;
})();