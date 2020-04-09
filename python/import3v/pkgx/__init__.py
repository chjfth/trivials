
print("[%s] __name__=%s"%(__file__,__name__))

# we do not refer to sub1 here

sub2 = "sub2 is string"

from . import sub3

from .sub4 import foo

print("__init__.py done.")
