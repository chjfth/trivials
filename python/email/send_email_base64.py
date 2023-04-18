#!/usr/bin/env python3
#coding: utf-8

import sys
# Import smtplib for the actual sending function
import smtplib
import email.encoders
import email.policy
from email.mime.text import MIMEText

email_server = sys.argv[1] if len(sys.argv)>1 else '192.168.5.11'
email_port = int(sys.argv[2]) if len(sys.argv)>2 else 25

emfrom = 'sender@foo.com'
emto =   'receiver@bar.com'

title = '李白《宣州谢朓楼饯别校书叔云》'
content = """弃我去者，昨日之日不可留。
乱我心者，今日之日多烦忧。
长风万里送秋雁，对此可以酣高楼。
蓬莱文章建安骨，中间小谢又清发。
俱怀逸兴壮思飞，欲上青天览明月。
抽刀断水水更流，举杯销愁愁更愁。
人生在世不称意，明朝散发弄扁舟。"""

# Create a text/plain message
mimemsg = MIMEText(
	content, 'plain', _charset='utf8', # Content-Type: text/plain; charset="utf8"
	policy=email.policy.SMTP)
   
assert mimemsg['Content-Transfer-Encoding'] == 'base64'
#
mimemsg['From'] = emfrom
mimemsg['To'] = emto
mimemsg['Subject'] = title

print("Sending mail to %s:%d ..."%(email_server, email_port))

# Send the message via our own SMTP server.
server = smtplib.SMTP(email_server, email_port)

mimestream = mimemsg.as_bytes()
server.sendmail(emfrom, emto, mimestream)

server.quit()
