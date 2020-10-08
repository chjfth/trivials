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
		logfiledir = "__logs__"
		os.makedirs(logfiledir, exist_ok=True)
		self.logfilepath = os.path.join(logfiledir, nowtimestr()+".ChsFile.log")
		self.logfh = open(self.logfilepath, 'a', encoding='utf8')
		super().__init__(self.log_once)

		self.filepath = os.path.abspath(filepath)
		self.log_once("Input-file: {}".format(self.filepath), True)

	def __del__(self):
		self.log_once("Done. excpt_count={}".format(self.excpt_count))
	
	def log_once(self, msg, is_print=False):
		line = "[{}]{}\n".format(nowtimestr(), msg)
		self.logfh.write(line)
		if is_print:
			print(line, end='')

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
		


