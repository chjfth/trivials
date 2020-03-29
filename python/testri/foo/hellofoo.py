from ..bar import hellobar
from . import foo2

def sayhello():
	print("hellofoo.")
	foo2.hellofoo2()
	hellobar.sayhello()

if __name__=='__main__':
	sayhello()
