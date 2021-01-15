#!/usr/bin/env python3
#coding: utf-8

import asyncio

async def f():
	print("f()>>>")
	coro = asyncio.sleep(1.0)
	await coro
	print("f()<<<")
	return 123

async def main():
	print("main()>>>")
	result = await f()
	print("main()<<<")
	return result

def use_coro(coro):
	ret = coro.send(None)
	pass

use_coro(main())
print("Program Done.")

"""
CMD$ python p34-await2.py
main()>>>
f()>>>
Program Done.
"""
