#!/usr/bin/env python3
#coding: utf-8

import os, sys, time
import traceback
from LoggerFence import LoggerFence

def nowtimestr():
	return time.strftime('%Y%m%d.%H%M%S', time.localtime())

class ChsFile(LoggerFence): # inherit from LoggerFence
	
	class Err(Exception):
		def __init__(self, msg):
			self.msg = msg

	def __init__(self, filepath):
		logfilepath = os.path.join("__logs__", nowtimestr()+".ChsFile.log")
		super().__init__(logfilepath)

		self.filepath = os.path.abspath(filepath)
		self.log_once("Processing: {}".format(self.filepath))

	def __del__(self):
		self.log_once("Done. excpt_count={}".format(self.excpt_count))

	@LoggerFence.mark_api
	def GetSize(self):
		filebytes = os.path.getsize(self.filepath)
		return filebytes
	
	@LoggerFence.mark_api
	def GetText(self):
		with open(self.filepath, "r", encoding="utf8") as fh:
			try:
				text = fh.read()
				return text
			except UnicodeDecodeError:
				self.log_once("Not UTF8: {}".format(self.filepath))
				pass
		
		with open(self.filepath, "r", encoding="gbk") as fh:
			try:
				text = fh.read()
				return text
			except UnicodeDecodeError:
				self.log_once("Not GBK : {}".format(self.filepath))
				pass
		
		raise __class__.Err("The file is in neither UTF8 or GBK.")
	
	@LoggerFence.mark_api
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

		bytes = chsfile.GetSize()
		bytes2, chars, text = chsfile.GetBoth()
		
		assert(bytes==bytes2)
		print(f"File bytes: {bytes}")
		print(f"Characters: {chars}")
		print(text)

	except ChsFile.Err as e:
		print("[ERROR] "+e.msg)

#	text = chsfile.GetText()
