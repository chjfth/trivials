# tweakref_cb1x.py
import sys, weakref
from sys import getrefcount
from weakref import getweakrefcount

def weakfin(wr):
	print("weakfin: id(wr)=%d , wr()=%s"%(id(wr), wr()))
	

class CC: pass

print(sys.executable)

r1 = CC()
print( "refcount =", getrefcount(r1) ) # 2
r2 = r1
print( "refcount =", getrefcount(r1) ) # 3

wr1 = weakref.ref(r1)
print( 'wr1 -> weakrefcount =', getweakrefcount(r1) ) # 1

wr2 = weakref.ref(r1, weakfin) 
print( 'wr2 -> weakrefcount =', getweakrefcount(r1) ) # now 2

wr1a = weakref.ref(r1)
print( 'wr1a-> weakrefcount =', getweakrefcount(r1) ) # still 2

print(f"{id(wr1) =}")
print(f"{id(wr1a)=}")
print(f"{id(wr2) =}")

print("del r1...")
del r1
print("del r2...")
del r2

print("==done==")