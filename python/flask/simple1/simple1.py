import time, datetime
from flask import Flask
from flask import make_response

app = Flask(__name__)

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

@app.route('/cookie1')
def cookie1():


	tzchina = datetime.timezone(datetime.timedelta(hours=8)) 
	dt_expire = datetime.datetime(2020, 12, 20, 9,2,3, 0, tzchina)

	resp = make_response('<p>Persistent cookies set.</p>')
	
	resp.set_cookie('mycook', 'yummy', expires=dt_expire)
	
	resp.set_cookie('mycook_with_path1', '1path', expires=dt_expire,
		path='/cookie1')
	#
	resp.set_cookie('mycook_with_path2', '2path', expires=dt_expire,
		path='/mypath')
	
	resp.set_cookie('mycook_with_domain', '3domain', expires=dt_expire,
		domain='.chjhost.com')
	
	resp.set_cookie('mycook_httponly', '4httponly', expires=dt_expire,
		httponly=True)
	
	resp.set_cookie('mycook_secure', '5secure', expires=dt_expire,
		secure=True)
	
	return resp
