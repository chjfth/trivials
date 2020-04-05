
__all__ = ["sayhello", "sayhowdy"] # mypkg1g

print("[%s] __name__=%s"%(__file__,__name__))

from .sayhello import *
from .sayhowdy import *
