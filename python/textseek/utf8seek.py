#!/usr/bin/env python3
#coding: utf-8

filename = "utf8seek.txt"

utf8bytes = b'\x41\xE7\x94\xB5\x42' # "A电B"
utf8chars = utf8bytes.decode("utf8")

with open(filename, "w", encoding="utf8") as fhnew:
	wret = fhnew.write(utf8chars)
	wtell = fhnew.tell()

	if wret==len(utf8bytes):
		print("For a text stream, write()'s return values is count of bytes.")
	elif wret==len(utf8chars):
		print("For a text stream, write()'s return values is count of characters.")
	else:
		raise ValueError("Got unexpected write() return")

	if wtell==len(utf8bytes):
		print("For a text stream, tell()'s return values is offset of bytes.")
	elif wtell==len(utf8chars):
		print("For a text stream, tell()'s return values is offset of characters.")
	else:
		raise ValueError("Got unexpected tell() return")

with open(filename, "r", encoding="utf8") as fhread:
	for seq in range(3):
		tellpos = fhread.tell()
		c = fhread.read(1)
		print("[%d] tellpos=%d , read-char=%s"%(seq, tellpos, c))

