#!/usr/bin/env python3
#coding: utf-8

import asyncio

async def f():
	try:
		while True: 
			await asyncio.sleep(0)
	except asyncio.CancelledError:
		print('I was cancelled!')
	else:
		return 111
coro = f()
coro.send(None)
coro.send(None)
coro.throw(asyncio.CancelledError)
