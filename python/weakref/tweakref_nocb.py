import weakref
from sys import getrefcount
from weakref import getweakrefcount

class CC: pass

r1 = CC()
print( "refcount =", getrefcount(r1) ) # 2
r2 = r1
print( "refcount =", getrefcount(r1) ) # 3

wr1 = weakref.ref(r1)
print( 'wr1 -> weakrefcount =', getweakrefcount(r1) ) # 1

wr2 = weakref.ref(r1) # or weakref.ref(r2)
print( 'wr2 -> weakrefcount =', getweakrefcount(r1) ) # still 1

if id(wr1)==id(wr2):
	print("weakref objs id same: %d"%(id(wr1)))
else:
	print("weakref objs id diff: %d , %d"%(id(wr1), id(wr2)))

