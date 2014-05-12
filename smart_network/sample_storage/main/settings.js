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

http.onrequest = function (request) {

  for (var i = 0, len = urlpatterns.length; i < len; ++i) {
     var pattern = urlpatterns[i];
	 print(pattern[0].test);
	 print(typeof(pattern[1]));
	 print(request.uri);
     if (pattern[0].test(request.uri)) {
	   return pattern[1](request);
	 }
  }

	var r = request;
	print(request);
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
	}

    //var res = new http.Response();
	//var res = new Response();
	var res = new this.Response();
	print(res);
    print(res.statusCode);
    res.statusCode = 500;
    print(res.statusCode);
    res.statusCode = 200;
    print(res.statusCode);

    
    res.setHeader('A', 'B');
    print(res.getHeader);
    print(res.getHeader('A'));
    res.removeHeader('A');
    print(res.getHeader('A'));

    print(res.data);
    res.data = 'A';
    print(res.data);
    res.data = 'B';
    print(res.data);
    res.data = 'C';
    print(res.data);
	
	r = [res, "ABC", null, undefined, NaN, http.Response];
	i = count++;
	if (count == r.length) {
		count = 0;
	}
	return r[i];

    //return res;
    //return "ABC";
    //return null;
    //return undefined;
    //return NaN;
    //return http.Response;
};
http.onmessage = function (message) {
	print(message);
    //http.send(message);
    http.notify(message);
};
http.onerror = function (error) {
	print('error');
};
print(http.listen());


print(http)
print(http.constructor);
print(http.constructor.prototpye);
