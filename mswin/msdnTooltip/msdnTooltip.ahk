; This is the AmHotkey module for doing stress testing, in order to see 
; whether msdnTooltip.exe leaks memory/resource after a long run.
; Include this .ahk at _more_includes_.ahk's first line like this:
;
;	#Include msdnTooltip.ahk
;
; Reload AmHotkey, get Start/Stop entrance from Appkey+w menu.

AUTOEXEC_msdnTooltip: 


Init_msdnTooltip()
; -- This function's body is defined after the first "return",
;    but we have to call it *before* the first "return".

return ; The first return in this ahk. It marks the End of auto-execute section.


#Include %A_LineFile%\..\winshell.ahk

Init_msdnTooltip()
{
	; Add [Start Test]/[Stop Test] to AppsKey context menu.

	dev_MenuAddItem(winshell.UtilityMenu, "# Start stress msdnTooltip", "Start_msdnTooltip")
	dev_MenuAddItem(winshell.UtilityMenu, "# Stop stress msdnTooltip", "Stop_msdnTooltip")
}


class msdnTooltipStress
{
	static count := -1
	static stopflag := true
}

Stop_msdnTooltip()
{
	msdnTooltipStress.stopflag := true
	msdnTooltipStress.count := -1
}

Start_msdnTooltip()
{
	if(not dev_WinWaitActive("msdnTooltip", 100))
	{
		dev_MsgBoxWarning("Please first run msdnTooltip.exe, make it foreground window, then try again.")
		return
	}


	if(msdnTooltipStress.count>=0)
		return ; already started

	msdnTooltipStress.stopflag := false
	msdnTooltipStress.count := 0
	
	Repeat_msdnTooltip()
}


Repeat_msdnTooltip()
{
	if(msdnTooltipStress.stopflag)
		return

	arBtntext := [ "<A> Tooltip for a control"
		, "<B> Tooltip for a Rect-area" 
		, "<C> Multiline tooltip" 
		, "<D> Tracking tooltip concise"
		, "<E> Tracking tooltip misc"
		, "<F> In-place tooltip"
		, "<G> In-place tooltip" ]

	for index,text in arBtntext
	{
		dev_ControlClick("msdnTooltipDemo", text)
	}

	wait_msec := 1000
	
	for index,dlgtitle in arBtntext
	{
		if(not dev_WinWait(dlgtitle, wait_msec) )
		{
			; This happens quite easily.
			AmDbg0(Format("#{}[WinWait -miss] {}", msdnTooltipStress.count, dlgtitle))
		}
		wait_msec := 10
	}
	
	dev_Sleep(50) ; close it after some millisec

	for index,dlgtitle in arBtntext
	{
		dev_WinClose(dlgtitle, wait_msec)
	}
	
	wait_msec := 200
	
	for index,dlgtitle in arBtntext
	{
		if(not dev_WinWaitClose(dlgtitle, wait_msec))
		{
			AmDbg0(Format("#{}[WinClose-miss] {}", msdnTooltipStress.count, dlgtitle))
		}
		wait_msec := 10
	}

	if(msdnTooltipStress.count >=0 )
	{
		msdnTooltipStress.count += 1
		AmDbg0(Format("#{} stressed.", msdnTooltipStress.count))
		
		dev_StartTimerOnce("Repeat_msdnTooltip", 50)
	}
	
	return true
}
