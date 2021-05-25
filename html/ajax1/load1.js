"use strict";

// Please prepare a host name "chjtest.com" on your DNS resolver to run this code.
//
// To serve the http request, we can use Python 3, run from the parent dir:
//
//	python -m http.server 8080
//
// and browse http://chjtest.com:8080/ajax1/ to go.

(function load1() {
	var xhr = new XMLHttpRequest();
	
	xhr.open("GET", "http://chjtest.com:8080/ajax1/data1.json");
	
	xhr.onload = function() {
		console.log("### Got json data: " + this.response);
	}
	
	xhr.send();
})();
