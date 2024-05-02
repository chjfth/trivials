#Include %A_LineFile%\..\prints.ahk

do_work()

return ; auto-exec end

^F1:: prints("Inject this message")

^c:: ExitApp


do_work()
{
	prints("Press Ctrl+F1 to inject a message.")
	prints("Press Ctrl+C to quit.")

	winti := "ahk_class Notepad"

	if(not WinExist(winti))
	{
		Run, Notepad
	}
	
	WinWait, % winti
	WinActivate,
	WinWaitActive
	
	WinGet, hwnd, ID
	
	fnobj := Func("TimerDo").Bind(hwnd)
	SetTimer, % fnobj, 1000
}

TimerDo(hwnd)
{
	static s_count := 0
	s_count++
	
;	Critical, On
	
	prints(Format("[#{}]", s_count))
	prints(Format("[#{}] Timer Do >>>", s_count))
	
	winti := "ahk_id " hwnd
	
	WinGetPos, x0, y0, w, h, % winti
	x1 := x0 + 10

	Loop, 3
	{
		WinMove, % winti, , x1, y0
		prints(Format("[#{}.{}] Move off", s_count, A_Index))
		
		WinMove, % winti, , x0, y0
		prints(Format("[#{}.{}] Move back", s_count, A_Index))
	}

	prints(Format("[#{}] Timer Do <<<", s_count))
}
