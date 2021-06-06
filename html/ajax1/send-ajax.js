"use strict";

// Please prepare a host name "chjtest.com" on your DNS resolver to run this code.
//
// To serve the http request, we can use Python 3, run from the parent dir:
//
//	python -m http.server 8080
//
// and browse http://chjtest.com:8080/ajax1/ to go.

(function send_my_ajax() {
	var xhr = new XMLHttpRequest();
	
	xhr.open("GET", "http://chjtest.com:8080/ajax1/data1.json");
	
	var origin = window.location.origin;
	
	xhr.onload = function() {
		// this.status==200 if success.
		console.log("[" + origin  + "]### Got json data: " + this.response);
	}

	xhr.onerror = function() {
		console.log("[" + origin  + "]!!! XHR Fail: status=" + this.status);
	}
	
	xhr.send();
})();
