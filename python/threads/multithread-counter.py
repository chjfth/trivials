#!/usr/bin/env python3
#coding: utf-8

import os, sys, time
from threading import Thread, Lock

COUNTS_PER_THREAD = 10
sleep_millisec = 0
use_lock = False
stat = {}


class CtxNull:
	def __enter__(self):
		return None
	def __exit__(self, exc_type, exc_val, exc_tb):
		return False
   

class Counter:
	def __init__(self):
		self._stars = 0
		self._lock = Lock()

	def inc(self):
		with self._lock if use_lock else CtxNull():
			old_stars = self._stars
			
			time.sleep(sleep_millisec/1000)
			
			new_stars = old_stars + 1
			self._stars = new_stars
			return new_stars

	@property
	def value(self):
		return self._stars

	def Count(self, n, tid):
		for i in range(n):
			new_stars = self.inc() # increase stars by one
			PrintProgress(tid, new_stars)
			

g_lock = Lock()
g_idx = 0
#
def PrintProgress(tid, new_stars):
	global g_idx
	with g_lock:
		g_idx += 1
		print("#{}. [tid-{}] => {}.".format(g_idx, tid, new_stars))
		if not new_stars in stat:
			stat[new_stars] = []
		stat[new_stars].append((g_idx, tid))

def PrintAbnormals():
	for i in sorted(stat, key=lambda x:len(stat[x])):
		if len(stat[i])>1:
			print("=> {} is counted by threads: {}".format(i, 
					', '.join([ "(#{})tid-{}".format(item[0], item[1]) for item in stat[i] ])
					))

def do_test(nthreads):
	# create a single counter to be shared by ALL threads
	counter = Counter()

	threads = []
	for i in range(nthreads):
		tid = i+1 # as thread id
		t = Thread(target=counter.Count, args=(COUNTS_PER_THREAD, tid))
		t.start()
		threads.append(t)
		time.sleep(sleep_millisec*0.7/1000)

	# wait all threads done
	for i in range(nthreads):
		threads[i].join()

	print("Counter final = {}".format(counter.value))
	#
	correct_final = nthreads * COUNTS_PER_THREAD
	diff = correct_final - counter.value
	if diff!=0:
		print("*"*50)
		print("PANIC! The Correct result is {}. Diff = {}".format(correct_final, diff))
		PrintAbnormals()
		print("*"*50)

if __name__ == '__main__':
	filenam = os.path.basename(__file__)
	if len(sys.argv)==1 or int(sys.argv[1])<=0:
		print("See many threads compete for a counter.")
		print("Usage:")
		print("  {} <threads> [sleep-millisec] [lock]".format(filenam))
		print("Example:")
		print("  {} 1    ".format(filenam))
		print("  {} 10   ".format(filenam))
		print("  {} 10  1".format(filenam))
		print("  {} 100 10".format(filenam))
		print("  {} 10   0 lock".format(filenam))
		print("Note:")
		print("  If omitted, sleep-millisec defaults to 0.")
		print("  Using 0 for sleep-millisec is very special, we'll see a very tiny final bias.")
		exit(1)

	nthreads = int(sys.argv[1])

	if len(sys.argv)>2:
		sleep_millisec = float(sys.argv[2])

	if len(sys.argv)>3 and sys.argv[3]=="lock":
		use_lock = True

	do_test(nthreads)
