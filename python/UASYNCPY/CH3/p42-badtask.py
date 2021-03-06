#!/usr/bin/env python3
#coding: utf-8

import asyncio
from contextlib import suppress

async def main(f: asyncio.Future):
	await asyncio.sleep(1)
	try:
		f.set_result('I have finished.')
	except RuntimeError as e:
		print(f'No longer allowed: {e}')
		f.cancel()

loop = asyncio.get_event_loop()
coro_sleep = asyncio.sleep(1_000_000)
fut = asyncio.Task(coro_sleep)
print(fut.done()) # False

coro_main = main(fut)
tsk_main = loop.create_task(coro_main)
# <Task pending name='Task-2' coro=<main() running at <console>:1>>

with suppress(asyncio.CancelledError):
	loop.run_until_complete(fut)

#No longer allowed: Task does not support set_result operation
print(fut.done()) #True

print(fut.cancelled()) # True


"""On Python 3.7.4:

False
No longer allowed: Task does not support set_result operation
True
True

"""
