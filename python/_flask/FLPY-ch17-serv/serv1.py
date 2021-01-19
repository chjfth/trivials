import time
from datetime import datetime,tzinfo,timedelta,timezone
from flask import Flask, abort
from flask import request, make_response
from flask import send_file


app = Flask(__name__)

tzchina = timezone(timedelta(hours=8)) 
dt_expire = datetime(2020, 12, 30, 9,2,3, 0, tzchina)

@app.route('/')
def index():
	return '<h1>Demo server for [FLPY] CH 17!</h1>'

@app.route('/data/flags/<cc1>/<filename>')
def serv_flag(cc1, filename):
	time.sleep(1.0) # 1 second sleep to simulate network delay
	
	if filename.endswith('.gif'):

		if cc1.upper()=='US': # simulate error
			abort(404)

		return "This is {}.gif".format(cc1)
		
	elif filename.endswith('.json'):
		
		body = """"tld_cc": ".{cc1}", 
			"iso_cc": "{cc1}", 
			"country": "Country name of {cc1}", 
			"gec_cc": "gec of {cc1}"
			""".format(cc1=cc1)
		body = "{" + body + "}"

		if cc1.upper()=='JP': # simulate error
			body = "MY WRONG JSON BODY"

		resp = make_response(body)
		resp.mimetype = "text/json"
		
		return resp
	else:
		return "What do you want?"

if __name__ == "__main__":
    app.run(host='0.0.0.0', port=80)
