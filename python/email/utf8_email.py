#!/usr/bin/env python3
#coding: utf-8

import sys
# Import smtplib for the actual sending function
import smtplib

# Import the email modules we'll need
from email.message import EmailMessage

email_server = sys.argv[1] if len(sys.argv)>1 else '192.168.5.11'
email_port = int(sys.argv[2]) if len(sys.argv)>2 else 25

content = """白日依山尽，黄河入海流。
欲穷千里目，更上一层楼。"""

# Create a text/plain message
msg = EmailMessage()
msg.set_content(content)

msg['Subject'] = '《登鹳雀楼》'
msg['From'] = 'sender@foo.com'
msg['To'] = 'receiver@bar.com'

print("Sending mail to %s:%d ..."%(email_server, email_port))

s = smtplib.SMTP(email_server, email_port)
s.send_message(msg)
s.quit()
