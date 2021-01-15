#!/usr/bin/env python3
#coding: utf-8

async def f():
	await asyncio.sleep(1.0)
	return 123

async def main():
	result = await f()
	return result

# Erroneous way to call main like this, just for experiment.
# RuntimeWarning: coroutine 'main' was never awaited
main()
