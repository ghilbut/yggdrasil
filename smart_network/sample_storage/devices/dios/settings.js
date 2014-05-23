count = 0;

function static_handler(request) {
print (http.Response);
  var res = new http.Response();
  print('STATIC');
  res.data = 'STATIC';
  return res;
}

urlpatterns = [
  [/^\/static\/[^?#]+($|[?#]{1}.*)/, static_handler]
]


print('############################################');
print(device);
print(device.constructor);
print(device.constructor.prototpye);

print(File);
print(File.constructor);
print(File.constructor.prototpye);

device.onservice = function (service) {
  print('======== device.onload ========');
  
  service.http.onrequest = function (request) {

	    var r = request;
	    /*print(request);
	    print(request.constructor);
	    print(request.constructor.prototype);
	
	    print(r.request_method);
	    print(r.uri);
	    print(r.http_version);
	    print(r.query_string);
	    print(r.remote_ip);
	    print(r.local_ip);
	    print(r.remote_port);
	    print(r.local_port);

	    var hs = r.headers;
	    for (var i = 0, len = hs.length; i < len; ++i) {
		    print('----------------------------------------');
		    var n = hs[i].name, v = hs[i].value;
		    print(hs[i]);
		    print(n);
		    print(v);
		    print(hs[n]);
		    print('----------------------------------------');
	    }*/

	    var res = new this.Response();
        res.statusCode = 200;
        res.setHeader('A', 'B');
        res.data = 'A';
	
	    //r = [res, "ABC", null, undefined, NaN, http.Response];
        r = [res, "ABC", null, undefined, NaN, this.Response];
	    i = count++;
	    if (count === r.length) {
		    count = 0;
	    }
	    return r[i];
    };

    var sock_ = false;
    service.http.onwebsocket = function (sock) {

        if (!sock_) {
            print('XXXXXXXXXXXX');
            sock_ = sock;
        }

        sock.onmessage = function (message) {
	        print(message);
            print('[1] ' + message);
            sock_.send('[SEND] ' + message);
            print(this === sock_);
            print('[2] ' + message);
            this.service.http.notify('[NOTIFY] ' + message);
        };
        sock.onclosed = function () {
	        print('closed');
            sock_ = false;
        }
    };
};