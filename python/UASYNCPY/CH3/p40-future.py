#!/usr/bin/env python3
#coding: utf-8

import asyncio

async def main(f: asyncio.Future):
	print("In main(), before sleep.")
	await asyncio.sleep(1)
	print("In main(), after sleep.")
	f.set_result('I have finished.')

loop = asyncio.get_event_loop()
fut = asyncio.Future()
print(fut.done()) # False

coro = main(fut)
tsk = loop.create_task(coro)
# <Task pending name='Task-1' coro=<main() running at <console>:1>>

loop.run_until_complete(fut)
# 'I have finished.'

print(fut.done()) # True

print(fut.result())
