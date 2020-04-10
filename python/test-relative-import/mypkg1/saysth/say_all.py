
print("[%s] __name__=%s"%(__file__,__name__))

from .. import share_util
from . import sayhello, sayhowdy

def say_all():
	share_util.print_share()
	sayhello()
	sayhowdy()

if __name__=='__main__':
	say_all()

