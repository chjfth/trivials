
print("[%s] __name__=%s"%(__file__,__name__))

def sayhowdy():
	print("Howdy~!")

# Would cause double-import warning if activated!
if __name__=='__main__':
	sayhowdy()

