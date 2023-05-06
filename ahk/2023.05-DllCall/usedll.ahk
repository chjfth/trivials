
global input10 := 10

call_BadDllName()
{
	sumhalf := DllCall("Bad.dll\BadEntry"
		, "int*", input
		, "int" , 5
		, "double")

	MsgBox, % "Bad.dll\BadEntry, ErrorLevel = " ErrorLevel ; -3
}

call_BadEntry()
{
	sumhalf := DllCall("chjdll.dll\BadEntry"
		, "int*", input
		, "int" , 5
		, "double")

	MsgBox, % "chjdll.dll\BadEntry, ErrorLevel = " ErrorLevel ; -4
}

use_intAst_ahkvar()
{
	sumhalf := DllCall("chjdll.dll\SumHalf"
		, "int*", input
		, "int" , 5
		, "double") ; good

	MsgBox, % A_ThisFunc Format(":() chjdll.dll\SumHalf, ErrorLevel={}, sumhalf={}"
		, ErrorLevel, sumhalf)
}


use_Ptr_ahkvar()
{
	sumhalf := DllCall("chjdll.dll\SumHalf"
		, "Ptr", input10
		, "int" , 5
		, "double") ; got ErrorLevel=0xC0000005

	MsgBox, % A_ThisFunc Format("(): chjdll.dll\SumHalf, ErrorLevel={}, sumhalf={}"
		, ErrorLevel, sumhalf)
}

use_Ptr_ahkvarAd()
{
	sumhalf := DllCall("chjdll.dll\SumHalf"
		, "Ptr", &input10
		, "int" , 5
		, "double") ; got wrong value

	MsgBox, % A_ThisFunc Format(":() chjdll.dll\SumHalf, ErrorLevel={}, sumhalf={}"
		, ErrorLevel, sumhalf)
}

use_Ptr_Cvar()
{
	VarSetCapacity(cvar, 4, 0)
	NumPut(input10, Cvar, "int")
	sumhalf := DllCall("chjdll.dll\SumHalf"
		, "Ptr", &Cvar
		, "int" , 5
		, "double") ; good

	MsgBox, % A_ThisFunc Format("(): chjdll.dll\SumHalf, ErrorLevel={}, sumhalf={}"
		, ErrorLevel, sumhalf)
}

call_BadDllName()
call_BadEntry()
use_intAst_ahkvar()
use_Ptr_ahkvar()
use_Ptr_ahkvarAd()
use_Ptr_Cvar()
