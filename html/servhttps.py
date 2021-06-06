#!/usr/bin/env python3

# Thanks: https://stackoverflow.com/a/19706670/151453
# openssl req -new -x509 -keyout localhost.pem -out localhost.pem -days 3650 -nodes

import http.server, ssl

server_address = ('0.0.0.0', 4433)
httpd = http.server.HTTPServer(server_address, http.server.SimpleHTTPRequestHandler)
httpd.socket = ssl.wrap_socket(httpd.socket,
                               server_side=True,
                               certfile='localhost.pem',
                               ssl_version=ssl.PROTOCOL_TLS)
httpd.serve_forever()
