#!/usr/bin/env python3
#coding: utf-8

import asyncio

async def f():
	count = 0
	try:
		while True: 
			await asyncio.sleep(0)
			count += 1
			print("[%d]Code run after await."%(count))
	except asyncio.CancelledError:
		print('I was cancelled!')
	else:
		return 111

coro = f()
coro.send(None)
coro.send(None)
coro.send(10)
coro.throw(asyncio.CancelledError)
