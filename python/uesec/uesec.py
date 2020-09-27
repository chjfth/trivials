#!/usr/bin/env python3
#coding: utf-8

"""uesec: Unix Epoch seconds.
"""

import os, sys, time, math

def do_work():
	if len(sys.argv)==1:
		uesecf = time.time() # get current time in uesec
	else:
		uesecf = float(sys.argv[1])
	
	frac, uesec = math.modf(uesecf)
	
	frac_us = ""
	if frac!=0:
		frac_us = " ."+("%.6f"%(frac))[2:]
	
	print("uesec=%d"%(uesec))
	print("UTC  =%s%s"%(
		time.strftime('%Y-%m-%d %H:%M:%S', time.gmtime(uesec)), 
		frac_us))
	print("Local=%s%s"%(
		time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(uesec)), 
		frac_us))

if __name__=='__main__':
	do_work()
