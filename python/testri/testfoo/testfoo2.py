import os, sys
dirtoppkg = os.path.dirname(os.path.dirname(os.path.abspath(__file__))) 
dirtoppkg = os.path.dirname(dirtoppkg)
print("dirtoppkg = %s"%(dirtoppkg)) # d:\test

sys.path.insert(0, dirtoppkg)

from testri.foo import hellofoo
hellofoo.sayhello()
