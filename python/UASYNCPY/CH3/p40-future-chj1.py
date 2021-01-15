#!/usr/bin/env python3
#coding: utf-8

# [2021-01-15] Chj tested with Python 3.7.4

import asyncio

async def main(f: asyncio.Future):
	print("In main(), before sleep.")
	await asyncio.sleep(1)
	print("In main(), after sleep.")
	f.set_result('I have finished.')
	
	# With this, statement, at SPOT-A, 
	# fut.done() will be True and tsk.done() will be false.
	await asyncio.sleep(0.01)

loop = asyncio.get_event_loop()
fut = asyncio.Future()
print("init fut.done()=", fut.done()) # False

coro = main(fut)
tsk = loop.create_task(coro)
# <Task pending name='Task-1' coro=<main() running at <console>:1>>

cpt = loop.run_until_complete(fut)
# cpt will be 'I have finished.'

print("* fut.done()=", fut.done()) # True
print("* tsk.done()=", tsk.done()) # False

# SPOT-A

print(fut.result())

# A second run of 
#	loop.run_until_complete(fut)
# or
#	loop.run_until_complete(tsk)
# here, will change tsk.done() to True.

