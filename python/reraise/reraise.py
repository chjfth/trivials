#!/usr/bin/env python3
#coding: utf-8

import os, sys

def task1():
	print("In task1().")
	
def task2():
	print("Start task2().")
	fh = open("somefile.txt", "r")
	chars = len(fh.read())
	print("End task2() %d chars."%(chars))
	
def task3():
	print("In task3().")


def job1():
	task1()
	task2()
	task3()

def job2():
	try:
		task1()
		task2()
		task3()
	except OSError:
		print("BAD! Digesting OSError.")
		raise;
	print("You cannot see this.")

if __name__=='__main__':
	print("Python version: {}.{}.{}".format(*sys.version_info[0:3]))
	
	if len(sys.argv)==2 and sys.argv[1]=='2':
		job2()
	
	else:
		job1()
