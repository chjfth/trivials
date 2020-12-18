import time, datetime
from flask import Flask
from flask import make_response
from flask import send_file

""" We need following hosts to accomplish these experiments.

10.22.3.84	chjhost.com
10.22.3.84	abc.chjhost.com
10.22.3.84	chjimg.com
10.22.3.84	chj3rd.com

Start Flask https server with command:

	python -m flask run --host=0.0.0.0 --cert=adhoc
	
Experiments record:

	https://www.evernote.com/l/ABUcJByCCDVFsqoj8YRpgL0i9UZzkBrOYag/
"""

app = Flask(__name__)

tzchina = datetime.timezone(datetime.timedelta(hours=8)) 
dt_expire = datetime.datetime(2020, 12, 20, 9,2,3, 0, tzchina)

@app.route('/')
def index():
	return '<h1>Hello World !</h1>'

@app.route('/delay/<int:millisec>')
def delay(millisec):
	time.sleep(millisec/1000)
	return '<p>Done %d millisec sleep.</p>'%(millisec)

@app.route('/cookie0')
def cookie0():
	resp = make_response('<p>Session-long cookie set.</p>')
	resp.set_cookie('mycook0.1', 'sweet01')
	resp.set_cookie('mycook0.2', 'sweet02')
	return resp

@app.route('/wantcookie')
def wantcookie():
	resp = make_response('<p>Cookies set from Flask server.</p>')
	
	resp.set_cookie('mycook_shortlive', 'short-lived')
	
	resp.set_cookie('mycook_persist', 'yum0', 
		expires=dt_expire)
	
	resp.set_cookie('mycook_path1', 'yum1', path='/wantcookie',
		expires=dt_expire,)
	#
	resp.set_cookie('mycook_path2', 'yum2', path='/cookie2',
		expires=dt_expire)
	
	resp.set_cookie('mycook_httponly', 'yum3', httponly=True,
		expires=dt_expire)
	
	resp.set_cookie('mycook_secure', 'yum4', secure=True,
		expires=dt_expire)
	
	resp.set_cookie('mycook_domain_attr', 'yum5', domain='chjhost.com',
		expires=dt_expire)
	
	return resp


@app.route('/cookie4')
def cookie4():
	resp = make_response('<p>Setting weird cookies.</p>')
	resp.set_cookie('Normal', 'NormalVal')
	resp.set_cookie('NormalWithSpace', 'Normal Value')
	resp.set_cookie('WithComma', 'abc,def')
	resp.set_cookie('WithSemicolon', 'xxx;yyy')
	return resp
	
@app.route('/cookover')
def cookover():

	resp = make_response('<p>Server Set-cookies without Domain=... attribute.</p>')
	
	resp.set_cookie('cookover', 'naturalYum', expires=dt_expire) 
	
	return resp

@app.route('/getimg/<int:idx>')
def getimg(idx):

	if idx==0:
		filename = 'cross.png'
	elif idx==1:
		filename = 'tick.png'
	else:
		idx = -1
		filename = 'question.png'
	
	resp = send_file(filename, mimetype='image/png')
	resp.set_cookie('mycook_last_img', str(idx), 
		domain='chjimg.com',
		expires=dt_expire)
	return resp


@app.route('/cookieimg')
def cookieimg():
	html = """\
<p>A page with 3rd-party image and cookie.</p>
<img src="http://chjimg.com:5000/getimg/0"/>
"""
	resp = make_response(html)
	resp.set_cookie('cook_htmlwithimg', 'yes')
	return resp


@app.route('/useiframe')
def useiframe():
	return in_useiframe()

@app.route('/useiframe_ssl')
def useiframe_ssl():
	return in_useiframe(True)

def in_useiframe(use_ssl=False):
	html = """\
<p>** A page with iframe and 3rd-party cookie.</p>
%s
<iframe src="%s://chj3rd.com:5000/cookie3rd"></iframe>
<p>== A page with iframe and 3rd-party cookie.</p>
"""%("", 'https' if use_ssl else 'http')
	resp = make_response(html)
	resp.set_cookie('cook_htmlwithiframe', 'yes', expires=dt_expire)
	return resp

@app.route('/cookie3rd')
def cookie3rd():
	resp = make_response('<p>Meet cookie from 3rd-party.</p>')
	resp.set_cookie('Meet3rd', 'Yum3rd',
		domain='chj3rd.com',
		samesite='None',
		secure=True,
		expires=dt_expire)
	return resp

