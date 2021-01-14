#!/usr/bin/env python3
#coding: utf-8

# quickstart.py
import asyncio, time

async def main():
	print(f'{time.ctime()} Hello!')
	await asyncio.sleep(1.0)
	print(f'{time.ctime()} Goodbye!')

asyncio.run(main())
