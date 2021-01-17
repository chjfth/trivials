import time
from datetime import datetime,tzinfo,timedelta,timezone
from flask import Flask
from flask import request, make_response
from flask import send_file


app = Flask(__name__)

tzchina = timezone(timedelta(hours=8)) 
dt_expire = datetime(2020, 12, 30, 9,2,3, 0, tzchina)

@app.route('/')
def index():
	return '<h1>Demo server for [FLPY] CH 17!</h1>'

@app.route('/data/flags/<cc1>/<cc2>.gif')
def serv_flag(cc1, cc2):
	time.sleep(1.0) # 1 second sleep to simulate network delay
	return "This is {}.gif".format(cc1)

if __name__ == "__main__":
    app.run(host='0.0.0.0', port=80)
