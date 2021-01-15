#!/usr/bin/env python3
#coding: utf-8

import asyncio

async def f():
	print("f() executing...")
	pass

coro = f()
loop = asyncio.get_event_loop()
task = loop.create_task(coro)
assert isinstance(task, asyncio.Task)

new_task = asyncio.ensure_future(coro)
assert isinstance(new_task, asyncio.Task)

mystery_meat = asyncio.ensure_future(task)
assert mystery_meat is task

#
# Chj try below:
#
loop.run_until_complete(task)
#
# -- This will cause error:
# RuntimeError: cannot reuse already awaited coroutine
#
# Error reason: task and new_task look like different "task" but 
# they contain the same coroutine object, which is not allowed.
#
# To fix, comment that `new_task = ...` line.
