
print("[%s] __name__=%s"%(__file__,__name__))

def sayhello():
	print("Hello~!")

# Would cause double-import warning! So comment out this following.
if __name__=='__main__':
	sayhello()

