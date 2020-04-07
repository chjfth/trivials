from ..bar import hellobar
from . import foo2

def sayhello():
	print("hellofoo.")
	foo2.hellofoo2()
	print("Full-qualified name of foo2 is: %s"%(foo2.__name__))
	hellobar.sayhello()

if __name__=='__main__':
	sayhello()
