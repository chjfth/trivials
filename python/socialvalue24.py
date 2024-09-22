#!/usr/bin/env python3
#coding: utf-8

import locale
import traceback

try:
	locale.setlocale(locale.LC_COLLATE, 'zh_CN.UTF-8') 
	# -- 'zh_CN.UTF-8' is available on Linux, Win10.21H2 (python 3.7+);
	#    but not available on Win7 python 3.8, would throw 
	#        locale.Error: unsupported locale setting
except:
	traceback.print_exc()

words1 = "富强 民主 文明 和谐 自由 平等 公正 法制 爱国 敬业 诚信 友善"
words1 += " 团结 繁荣 理性" # this is for quiz

words = sorted(words1.split(), key=locale.strxfrm)

for idx in range(len(words)):
	if idx%4==0:
		print("")
	print("(%2d)%s "%(idx+1, words[idx]), end='')

print("")
