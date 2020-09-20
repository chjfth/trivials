#!/usr/bin/env python3
#coding: utf-8

import os, sys, time

class ChsFile:
	
	class Err(Exception):
		def __init__(self, msg):
			self.msg = msg

	def __init__(self, filepath):
		self.filepath = filepath

	def GetText(self):
		with open(self.filepath, "r", encoding="utf8") as fh:
			try:
				text = fh.read()
				return text
			except UnicodeDecodeError:
				pass
		
		with open(self.filepath, "r", encoding="gbk") as fh:
			try:
				text = fh.read()
				return text
			except UnicodeDecodeError:
				pass
		
		raise ChsFile.Err("The file is in neither UTF8 or GBK.")
	
	def GetBoth(self):
		filebytes = os.path.getsize(self.filepath)
		
		text = self.GetText()
		nchars = len(text)
		
		return filebytes, nchars, text
		

if __name__=='__main__':
	print("Python version: {}.{}.{}".format(*sys.version_info[0:3]))
	if len(sys.argv)==1:
		print("Input a file name as parameter. The file should be a UTF8 or GBK text file.")
		exit(1)
	
	filepath = sys.argv[1]
	
	try:
		chsfile = ChsFile(filepath)

		bytes, chars, text = chsfile.GetBoth()
		print(f"File bytes: {bytes}")
		print(f"Characters: {chars}")
		print(text)

	except ChsFile.Err as e:
		print("[ERROR] "+e.msg)
