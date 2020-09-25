import os, sys, time
import traceback

def nowtimestr():
	return time.strftime('%Y%m%d.%H%M%S', time.localtime())

class LoggerFence:

	class Err(Exception):
		def __init__(self, msg):
			self.msg = msg

	def __init__(self, logfilepath="", is_append=False, fd=None):
		if fd!=None:
			self.__logfh = fd
		else:
			self.__logfilepath = os.path.abspath(logfilepath)
			self.logfile_create(is_append)
			assert(self.__logfh)

		self.nested = 0 # not private, can improve?
		self.excpt_count = 0

	def logfile_create(self, is_append=False):
		try:
			dirpath = os.path.dirname(self.__logfilepath)
			os.makedirs(dirpath, exist_ok=True)

			flag = 'a' if is_append else 'w'
			self.__logfh = open(self.__logfilepath, flag, encoding='utf8')

		except OSError:
			raise __class__.Err("Cannot open/create logfile: {}".format(self.__logfilepath))


	def log_once(self, msg, is_print=False):
		line = "[{}]{}\n".format(nowtimestr(), msg)
		self.__logfh.write(line)
		if is_print:
			print(line)

	def mark_api(func):
		# func is an unbound function
		def wrapper(self, *args, **kwargs):
			self.nested += 1
			try:
				ret = func(self, *args, **kwargs)
			except:
				# We log the exception to this-module logfile and let it propagate.
				if self.nested == 1:
					excpt_text = traceback.format_exc()
					self.log_once(excpt_text)

					self.excpt_count += 1
				raise
			finally:
				self.nested -= 1
			return ret

		return wrapper

