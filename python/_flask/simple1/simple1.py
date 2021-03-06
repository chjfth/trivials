import time
from datetime import datetime,tzinfo,timedelta,timezone
from flask import Flask
from flask import request, make_response
from flask import send_file

""" Flask launching envvar:

	FLASK_APP=simple1.py
	FLASK_ENV=development

Start Flask https server with command:

	python -m flask run --host=0.0.0.0 --cert=adhoc

We need following hosts to accomplish cookie related experiments.

10.22.3.84	chjhost.com
10.22.3.84	abc.chjhost.com
10.22.3.84	chjimg.com
10.22.3.84	chj3rd.com

Experiments record:

	https://www.evernote.com/l/ABUcJByCCDVFsqoj8YRpgL0i9UZzkBrOYag/
"""

app = Flask(__name__)

tzchina = timezone(timedelta(hours=8)) 
dt_expire = datetime(2020, 12, 30, 9,2,3, 0, tzchina)

@app.route('/')
def index():
	return '<h1>Hello World from Flask!</h1>'

@app.route('/utc')
def utc():
	uesec = time.time()
	dt0 = datetime.fromtimestamp(uesec, tz=timezone.utc)
	line1 = 'UTC:       %s'%(dt0.strftime('%Y-%m-%d.%H:%M:%S.%f'))
	line2 = 'Timestamp: %f'%(uesec)
	html = """<pre>{}
{}</pre>""".format(line1, line2)
	return html

@app.route('/test/<tail>')
def test(tail):
	return 'Tail is %d characters: %s'%(len(tail), tail)

@app.route('/delay/<int:millisec>')
def delay(millisec):
	time.sleep(millisec/1000)
	return '<p>Done %d millisec sleep.</p>'%(millisec)

@app.route('/burn/<int:millisec>')
def burn(millisec):
	# This consumes whole core of CPU
	uesec_end = time.time() + millisec/1000
	while time.time() <= uesec_end:
		pass
	return '<p>Done %d millisec CPU burning.</p>'%(millisec)

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

@app.route('/image/<int:idx>')
def image(idx):

	if idx==0:
		filename = 'cross.png'
	elif idx==1:
		filename = 'tick.png'
	else:
		idx = -1
		filename = 'question.png'
	
	resp = send_file(filename, mimetype='image/png')
	resp.set_cookie('mycook_last_image_idx', str(idx), 
		domain='chjimg.com',
		expires=dt_expire)
	return resp


@app.route('/wantimage<int:idx>')
def wantimage(idx):
	html = """\
<p>A page with 3rd-party image and cookie.</p>
<img src="%s://chjimg.com:5000/image/%d"/>
"""%(request.scheme, idx)
	resp = make_response(html)
	resp.set_cookie('cook_wantimage', 'yes')
	return resp


@app.route('/useiframe')
def useiframe():
	html = """\
<p>** A page with iframe and 3rd-party cookie.</p>
<iframe src="%s://chj3rd.com:5000/cookie3rd"></iframe>
<p>== A page with iframe and 3rd-party cookie.</p>
"""%(request.scheme)
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


if __name__ == "__main__":
	# Note: processes=3 is supported on Unix only.
    app.run(host='0.0.0.0', threaded=False, processes=3)
