import sys
import weakref
import gc

class MyString:
	def __init__(self, s):
		self.s = s
	def __str__(self):
		return self.s


def my_setdictval(idict, key, value):
	idict[key] = value

def my_dumpdict(idict_varname, idict):
	print("[%s]" % idict_varname)
	for key in idict:
		print("  %s : '%s'"%(key, idict[key]))

def test_weakref():
	dweakkey = weakref.WeakKeyDictionary()
	dweakval = weakref.WeakValueDictionary()

	key1 = MyString("weakkey1")
	my_setdictval(dweakkey, key1, "normal value")
	# -- We cannot just write:
	#      my_setdictval(dweakkey, "weakkey1", "normal value")
	# bcz dweakkey object would internally create a weakref obj from "weakkey1",
	# and that is not allowed:
	#  TypeError: cannot create weak reference to 'str' object.

	val1 = MyString("a weak value")
	my_setdictval(dweakval, "normkey1", val1)

	print("Probe 1:")
	my_dumpdict('dweakkey', dweakkey)
	my_dumpdict('dweakval', dweakval)

	del key1
	del val1

	print("Probe 2:")
	my_dumpdict('dweakkey', dweakkey)
	my_dumpdict('dweakval', dweakval)

	gc.collect()

	print("Probe 3:")
	my_dumpdict('dweakkey', dweakkey)
	my_dumpdict('dweakval', dweakval)


if __name__=='__main__':
	print(sys.executable)
	test_weakref()
