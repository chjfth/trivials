#!/usr/bin/env python3
#coding: utf-8
import sys
# Import smtplib for the actual sending function
import smtplib

# Create a text/plain message
emfrom = 'sender@foo.com'
emto = 'user1@zjb.lab'

eml_file = sys.argv[1] if len(sys.argv)>1 else 'test0D0A.eml'
email_server = sys.argv[2] if len(sys.argv)>2 else '192.168.5.11'
email_port = int(sys.argv[3]) if len(sys.argv)>3 else 25

print('Sending "%s" to %s:%d ...'%(eml_file, email_server, email_port))

eml = open(eml_file, 'rb').read()

s = smtplib.SMTP(email_server, email_port)
s.sendmail(emfrom, emto, eml)
s.quit()
