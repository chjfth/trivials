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
coro = asyncio.sleep(1_000_000)
fut = asyncio.Task(coro)
print(fut.done()) # False

tsk = loop.create_task(main(fut))
# <Task pending name='Task-2' coro=<main() running at <console>:1>>

with suppress(asyncio.CancelledError):
	loop.run_until_complete(fut)

#No longer allowed: Task does not support set_result operation
print(fut.done()) #True

print(fut.cancelled()) # True
