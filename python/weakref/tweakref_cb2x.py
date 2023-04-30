import weakref
from sys import getrefcount
from weakref import getweakrefcount

def weakfin(wr):
	print("weakfin: id(wr)=%d , wr()=%s"%(id(wr), wr()))
	

class CC: pass

r1 = CC()
print( "refcount =", getrefcount(r1) ) # 2
r2 = r1
print( "refcount =", getrefcount(r1) ) # 3

wr1 = weakref.ref(r1, weakfin)
print( 'wr1 -> weakrefcount =', getweakrefcount(r1) ) # 1

wr2 = weakref.ref(r1, weakfin) 
print( 'wr2 -> weakrefcount =', getweakrefcount(r1) ) # now 2

if id(wr1)==id(wr2):
	print("weakref objs id same: %d"%(id(wr1)))
else:
	print("weakref objs id diff: %d , %d"%(id(wr1), id(wr2)))

print("del r1...")
del r1
print("del r2...")
del r2

print("==done==")