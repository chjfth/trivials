
print("[%s] __name__=%s"%(__file__,__name__))

from . import * 
#	* is determined by __init__.py -> __all__[] list

def say_all():
	sayhello()
	sayhowdy()

if __name__=='__main__':
	say_all()

