
__all__ = ["sayhello", "sayhowdy", "dumbit"] # mypkg1e

print("[%s] __name__=%s"%(__file__,__name__))

from .sayhello import *
from .sayhowdy import *
