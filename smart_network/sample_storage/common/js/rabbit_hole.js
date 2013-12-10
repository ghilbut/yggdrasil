var rabbitHole = (function () {
	var rh_ = {
		Request: Request,
		notify: notify,
		onevent: false
	};

	var ws = false;

	function Request(query) {
		var self = this
			, xhr = new XMLHttpRequest()
			, query = query
			, params = false;

		if (typeof(query) !== 'string') {
			throw new TypeError('[ERROR] type of query should be string.');
		}

		xhr.onreadystatechange = resultCallback;
		xhr.open('POST', '/do/' + query, false);
		xhr.setRequestHeader("Content-Type", "text/plain;charset=UTF-8");

		self.setParam = function (key, value) {
			var type = typeof(key);
			if (type !== 'string') {
				throw new TypeError('[ERROR] type of key should be string.');
			}

			type = typeof(value);
			if (value !== null
				&& type !== 'string'
				&& type !== 'number'
				&& type !== 'boolean') {
				throw new TypeError('[ERROR] type of value should be one of null, string, number or boolean.');	
			}

			if (!params) {
				params = {};
			}

			params[key] = value;
		};
		self.send = function () {
			var data = { query: query, params: params };
			xhr.send(params ? JSON.stringify(data) : null);
		};

		self.onresponse = false;
		self.onerror = false;

		function resultCallback() {
			if (this.readyState !== this.DONE) {
				return;
			}

			if (this.status === 200) {
				if (self.onresponse && typeof(self.onresponse) === 'function') {
					self.onresponse(this.response)
				}
			} else {
				// TODO(ghilbut): it is not designed which how to notify error, yet.
				if (self.onerror && typeof(self.onerror) === 'function') {
					self.onerror();
				}
			}
		}
	}

	function notify(data) {
		if (typeof(data) !== 'string') {
			if (data.constructor !== Object) {
				throw new TypeError('[ERROR] type of data should be string or object.');
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
		// ping websocket
		if (ws && ws.readyState === ws.OPEN) {
			ws.send('');
		}
	}, 3000);

	document.aaa = function () {
		alert('bbb');
	};

	return rh_;
})();