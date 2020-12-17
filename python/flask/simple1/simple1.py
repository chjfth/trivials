import time, datetime
from flask import Flask
from flask import make_response

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
	
@app.route('/cookie5')
def cookie5():
	resp = make_response('<p>Meet cookie5.</p>')
	resp.set_cookie('Meet5', 'Yum5', expires=dt_expire, domain='chjhost.com')
	return resp
