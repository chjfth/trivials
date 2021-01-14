#!/usr/bin/env python3
#coding: utf-8

# quickstart_exe.py
import time
import asyncio

async def main():
	print(f'{time.ctime()} Hello!')
	await asyncio.sleep(1.0)
	print(f'{time.ctime()} Goodbye!')

def blocking():     # (1)
	time.sleep(0.5) # (2)
	print(f"{time.ctime()} Hello from a thread!")

loop = asyncio.get_event_loop()
task = loop.create_task(main())

loop.run_in_executor(None, blocking) # (3)
loop.run_until_complete(task)

pending = asyncio.all_tasks(loop=loop) # (4)

for task in pending:
	task.cancel()

group = asyncio.gather(*pending, return_exceptions=True)

loop.run_until_complete(group)
loop.close()
