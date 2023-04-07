#!/usr/bin/env python3
#coding: utf-8

"""This server listens for SMTP connection(port as first parameter), 
and save received email to disk file by current timestamp.
"""

import sys
import smtpd
import asyncore
import time
from datetime import datetime,tzinfo,timedelta,timezone

listen_port = int(sys.argv[1]) if len(sys.argv)>1 else 25000

class CustomSMTPServer(smtpd.SMTPServer):
	
	def process_message(self, peer, mailfrom, rcpttos, data, **kwargs):
		print('Receiving message from: %s'%(str(peer)))
		print('Message addressed from: %s'%mailfrom)
		print('Message addressed to  : %s'%(rcpttos))
		print('Message length        : %d bytes'%len(data))
		
		dtnow = datetime.now() # localtime
		filename = '%s-%s'%(dtnow.strftime('%Y%m%d.%H%M%S'), mailfrom)
		print('Saving to: %s'%(filename))
		with open(filename, 'wb') as fh:
			fh.write(data)
		return

print('Email server listening on port %d ...'%(listen_port))

server = CustomSMTPServer(('0.0.0.0', listen_port), None, enable_SMTPUTF8=True)

asyncore.loop()
