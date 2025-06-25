; This is the AmHotkey module for doing stress testing, in order to see 
; whether testDlgTooltipEasy.exe leaks memory/resource after a long run.
; Include this .ahk at _more_includes_.ahk's first line like this:
;
;	#Include testDlgTooltipEasy.ahk
;
; Reload AmHotkey, get Start/Stop entrance from Appkey+w menu.

AUTOEXEC_testDlgTooltipEasy: 


Init_testDlgTooltipEasy()
; -- This function's body is defined after the first "return",
;    but we have to call it *before* the first "return".

return ; The first return in this ahk. It marks the End of auto-execute section.


#Include %A_LineFile%\..\winshell.ahk

Init_testDlgTooltipEasy()
{
	; Add [Start Test]/[Stop Test] to AppsKey context menu.

	dev_MenuAddItem(winshell.UtilityMenu, "# Start stress testDlgTooltipEasy", "Start_testDlgTooltipEasy")
	dev_MenuAddItem(winshell.UtilityMenu, "# Stop stress testDlgTooltipEasy", "Stop_testDlgTooltipEasy")
}


class CtestDlgTooltipEasy
{
	static s_count := 0
}

DlgTooltipEasyTest1_once()
{
	CtestDlgTooltipEasy.s_count++

	AmDbg0(Format("Stage 1 (count: {})", CtestDlgTooltipEasy.s_count))
	
	hdlg := dev_GetHwndByWintitle("testDlgTooltipEasy")

	AmDbg0("Stage 2")
	
	dev_ClickInChildClassnn(hdlg, "&Add EasyTooltip", 60, 10, true)
	dev_Sleep(200)
	
	AmDbg0("Stage 3")
	
	dev_MouseMoveInChildClassnn(hdlg, "Edit1", 110, 100)
	dev_Sleep(200)

	AmDbg0("Stage 4")

	dev_ControlFocus("ahk_id " hdlg, "Edit1")
	dev_Sleep(200)
	dev_ControlFocus("ahk_id " hdlg, "Button1")
	dev_Sleep(200)

	AmDbg0("Stage 5")
	
	dev_ClickInChildClassnn(hdlg, "&Del EasyTooltip", 60, 10, true) 
	dev_Sleep(200)

	AmDbg0("Stage 6")
	
	dev_MouseMoveInChildClassnn(hdlg, "Edit1", 260, 100, true)
	dev_ControlSetText_hc(hdlg, "Edit1", Format("AHK auto cycles: {}", CtestDlgTooltipEasy.s_count))
	dev_Sleep(200) ; usage tooltip should not appear
	
}

Start_testDlgTooltipEasy()
{
	if(not dev_WinWaitActive("testDlgTooltipEasy", 100))
	{
		dev_MsgBoxWarning("Please first run testDlgTooltipEasy.exe, make it foreground window, then try again.")
		return
	}


	AmDbg0("Stage 0")	

	CtestDlgTooltipEasy.s_count := 0
	dev_StartTimerPeriodic("DlgTooltipEasyTest1_once", 2500, true)
;	dev_StartTimerOnce("DlgTooltipEasyTest1_once", 2000)
}

Stop_testDlgTooltipEasy()
{
	dev_StopTimer("DlgTooltipEasyTest1_once")
}

