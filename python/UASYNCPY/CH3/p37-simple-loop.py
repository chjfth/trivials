#!/usr/bin/env python3
#coding: utf-8

import asyncio

async def f():
	await asyncio.sleep(0)
	return 111

loop = asyncio.get_event_loop()
coro = f()
result = loop.run_until_complete(coro)

print("result=%s"%(result))
