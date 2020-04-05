
print("[%s] __name__=%s"%(__file__,__name__))

def sayhello():
	print("Hello~!")

# Would cause double-import warning!
if __name__=='__main__':
	sayhello()

