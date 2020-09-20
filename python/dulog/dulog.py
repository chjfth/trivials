#!/usr/bin/env python3
#coding: utf-8

import os, sys, time

def nowtimestr():
	return time.strftime('%Y%m%d.%H%M%S', time.localtime())

start_ts = nowtimestr()

class ChsFile:
	
	logfilenam = start_ts + ".ChsFile.log"
	logfiledir = "__logs__"
	logfh = None
	
	class Err(Exception):
		def __init__(self, msg):
			self.msg = msg

	@classmethod
	def CreateModuleLogfile(cls):
		if not cls.logfh:
			try:
				logfilepath = os.path.join(cls.logfiledir, cls.logfilenam)
				os.makedirs(cls.logfiledir, exist_ok=True)
				cls.logfh = open(logfilepath, 'w', encoding='utf8')
			except OSError:
				raise cls.Err("Cannot create logfile {}".format(logfilepath))
	
	@classmethod
	def mlog(cls, msg, isprint=False):
		line = "[{}]{}\n".format(nowtimestr(), msg)
		assert(cls.logfh)
		cls.logfh.write(line)
		if isprint:
			print(line)

	def __init__(self, filepath):
		__class__.CreateModuleLogfile() # Create ChsFile-module logfile first
		self.filepath = filepath        # Record protagonist filepath

	def GetText(self):
		with open(self.filepath, "r", encoding="utf8") as fh:
			try:
				text = fh.read()
				return text
			except UnicodeDecodeError:
				__class__.mlog("Not UTF8: {}".format(self.filepath))
				pass
		
		with open(self.filepath, "r", encoding="gbk") as fh:
			try:
				text = fh.read()
				return text
			except UnicodeDecodeError:
				__class__.mlog("Not GBK : {}".format(self.filepath))
				pass
		
		raise __class__.Err("The file is in neither UTF8 or GBK.")
	
	def GetSize(self):
		filebytes = os.path.getsize(self.filepath)
		return filebytes
	
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
