#!/usr/bin/env python3
# rsync rolling-checksum algorithm from https://rsync.samba.org/tech_report/node3.html

Xs = "0123456789abcdef"
M = 2**16 # can be any value

xlen = len(Xs)

X = []
for c in Xs:
	X.append(ord(c))

def a(k, l):
	sum = 0
	for i in range(k, l+1):
		sum += X[i]
	return sum % M

def b(k, l):
	sum = 0
	for i in range(k, l+1):
		prod = (l-i+1) * X[i]
		sum += prod
	return sum % M

def s(k, l):
	aa = a(k, l)
	bb = b(k, l)
	return aa + bb*M

def nexta(k, l):
	nxt = a(k,l) - X[k] + X[l+1]
	return nxt % M

def nextb(k, l):
	nxt = b(k,l) - (l-k+1)*X[k] + a(k+1,l+1)
	return nxt % M

def nexts(k, l):
	""" Rolling checksum: Its value will equal to s(k-1,l-1) """
	aa = nexta(k, l)
	bb = nextb(k, l)
	return aa + bb*M

def verify_all():
	count = 0
	for blocksize in (1,3,8):
		for offset in range(1, xlen+1-blocksize):
			end = offset+blocksize-1
			print("#%d Verifying s[%d,%d]... "%(count+1, offset, end), end='')

			s0 = s(offset, end)
			s1 = nexts(offset-1, end-1)

			# ASSERT whether the algorithm is correct.
			assert s0==s1

			print("%d (0x%X)"%(s0, s0))
			count += 1
	return count

#k=4; l=15
#print( s(k,l) , "|" , nexts(k-1,l-1))

if __name__=="__main__":
	count = verify_all()
	print("Sample content size: %d bytes"%(xlen))
	print(count, "cases verified OK.")

