int func2()
{
	int retv = 0;

	__asm {
		mov eax, 0x201;
		mov ebx, 0x202;

		add retv, eax;
		add retv, ebx;
	}
	return retv;
}

int func1()
{
	__asm {
		mov eax, 0x11;
		mov ebx, 0x12;
	}

	int retv = func2();
}

int main()
{
	int retv = func1();
	return 0;
}

