#!/usr/bin/env python3
#coding: utf-8

import os, sys
from ChsFile import ChsFile

if __name__=='__main__':
	print("Python version: {}.{}.{}".format(*sys.version_info[0:3]))
	if len(sys.argv)==1:
		print("Input a file name as parameter. The file should be a UTF8 or GBK text file.")
		exit(1)
	
	filepath = sys.argv[1]
	
	try:
		chsfile = ChsFile(filepath)

		bytes = chsfile.GetSize()
		bytes2, chars, text = chsfile.GetBoth()
		
		assert(bytes==bytes2)
		print(f"File bytes: {bytes}")
		print(f"Characters: {chars}")
		print(text)

	except ChsFile.Err as e:
		print("[ERROR] "+e.msg)

