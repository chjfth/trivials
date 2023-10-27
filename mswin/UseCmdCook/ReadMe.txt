[2023-10-27] 

This project creates a CmdCook.dll that export a DLL function named Cook1,
which can be called by rundll32.exe .

Thanks to: https://stmxcsr.com/micro/rundll-parse-args.html

For example:

	rundll32.exe CmdCook.dll,Cook1 one 22

Using DbgView.exe, we can see that Cook1() function outputs a string,
via OutputDebugString, like this:

	[Cook1] hwnd=0x00231044, hinst=0xBF0000, lpszCmdLine=one 22

Note: 
	rundll32.exe runs as a GUI program(not CUI), so we cannot
	use printf to output message.
