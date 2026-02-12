#!/usr/bin/env python3
# coding: utf-8

# This can be run on an Android phone, via PyDroid3 app.
# In PyDroid3, selecting this file to run, and the directory of this py file
# will be set as HTTP server root. 
# Verified on XiaoMi Mi11 Ultra, MIUI 13.

import os, sys
import http.server
from functools import partial

servport = 8888

if len(sys.argv)==1:
	server_root = os.getcwd()
else:
	server_root = os.path.abspath(sys.argv[1])

handler = partial(
    http.server.SimpleHTTPRequestHandler,
    directory=server_root
)

server_address = ('0.0.0.0', servport)
httpd = http.server.HTTPServer(server_address, handler)

print("Serving HTTP on %d ..."%(servport))
print("ServerRoot: %s"%(server_root)) 

httpd.serve_forever()
