#!/usr/bin/env python3
#coding: utf-8

import os, sys

from enum import Enum

class Method(Enum):
	Tight = 0
	Loose = 1

class GcdProgress:
	def __init__(self, a, b, method):
		if a<b: 
			a, b = b, a # swap
		self.a = a
		self.b = b
		
		self.method = method
		self.answer = 0
		
	def StepOnce(self):
		if self.answer!=0:
			return ""
		
		q = self.a // self.b
		r = self.a % self.b
		
		text ="{} / {} = {} ... {}".format(self.a, self.b, q, r)

		if r==0:
			self.answer = self.b
		
		if self.method==Method.Tight:
			self.a, self.b = self.b, r
		else:
			self.b = r

		return text

def FindGcd(a, b):

	gp = [GcdProgress(a, b, Method.Tight), GcdProgress(a, b, Method.Loose)]
	
	seq = 0
	
	while True:
		
		text0 = gp[0].StepOnce()
		text1 = gp[1].StepOnce()
		
		seq += 1
		prefix = "[{}]".format(seq)

		print("{0:<5}{1:<36}{2:36}".format(prefix, text0, text1))

		if gp[0].answer and gp[1].answer:
			break;
	
	print("Answer: {}".format(gp[0].a))
	
	if( gp[0].answer!=gp[1].answer ):
		print("BUT the Tight method results in WRONG answer({})!".format(gp[1].answer))
	

if __name__=='__main__':
	print("Given two numbers, calculate their GCD(greatest common denominator).")

	while True:
		ituple = input("Input number pair, separated by comma: ")
		a, b = ituple.split(',')
		FindGcd(int(a), int(b))
		print()