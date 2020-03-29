import os, sys
dirtoppkg = os.path.dirname(os.path.dirname(os.path.abspath(__file__))) 
print("dirtoppkg = %s"%(dirtoppkg)) # d:\test\testri

sys.path.insert(0, dirtoppkg)

from foo import hellofoo
hellofoo.sayhello()
