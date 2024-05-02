
class CTimeGapTeller
{
	_gap_millisec := 0
	_msec_prev := 0
	
	__New(gap_millisec)
	{
		this._msec_prev := 0
		this._gap_millisec := gap_millisec
	}
	
	CheckGap()
	{
		; If _gap_millisec time-period has elapsed since previous CheckGap() call,
		; return positive value, otherwise, return 0. 
		; If the positive value is N, it means N times of _gap_millisec has elapsed.
		; First-run returns 0.
		
		ret := 0 ; assume false
		now_msec := A_TickCount
		
		if(this._msec_prev>0)
		{
			elapsed_ms := now_msec - this._msec_prev
			if(elapsed_ms >= this._gap_millisec)
				ret := elapsed_ms // this._gap_millisec
		}

		this._msec_prev := now_msec
		return ret
	}
}

dev_Ts14AddSeconds(tsinput, seconds)
{
	; Add seconds to AHK 14-char timestamp (YYYYMMDDhhmmss).
	; A_Now has this format.
	;
	; seconds can be positive or negative
	
	tsoutput := tsinput
	EnvAdd, tsoutput, % seconds, Seconds
	return tsoutput
}

prints_AddTimestamp(msg)
{
	msg := StrReplace(msg, "`r`n", "`n")
	msg := StrReplace(msg, "`n", "`r`n")
	
	; I will report millisecond fraction, so need some extra work.
	;
	static s_start_msec   := A_TickCount
	static s_start_ymdhms := A_Now
	static s_prev_msec    := s_start_msec
	
	static s_prev_modu := ""
	
	now_tick := A_TickCount
	msec_from_prev := now_tick - s_prev_msec

	sec_from_start := (A_TickCount-s_start_msec) // 1000
	msec_frac := Mod(A_TickCount-s_start_msec, 1000)
	
	now_ymdhsm := dev_Ts14AddSeconds(s_start_ymdhms, sec_from_start)

	; now_ymdhsm is like "20221212115851"
	ymd  := substr(now_ymdhsm, 1, 8)
	hour := substr(now_ymdhsm, 9, 2)
	minu := substr(now_ymdhsm, 11, 2)
	sec  := substr(now_ymdhsm, 13, 2)
	
	stimestamp := Format("{}_{}:{}:{}.{:03}", ymd, hour, minu, sec, msec_frac)
	stimeplus  := Format("+{}.{:03}s", msec_from_prev//1000, Mod(msec_from_prev,1000)) ; "+1.002s" etc
	
	linemsg := Format("{1}[{2}] ({3}) {4}"
		, "", stimestamp, stimeplus, msg)
	
    s_prev_msec := now_tick

	if(modu==s_prev_modu) {
		is_same_modu_as_prev := true
	}
	else {
		is_same_modu_as_prev := false
		s_prev_modu := modu
	}
	
	return linemsg
}

prints(linemsg)
{	
	static s_tgt := new CTimeGapTeller(1000)

	linemsg := prints_AddTimestamp(linemsg)

	nsecs := s_tgt.CheckGap()
	dots := ""
	Loop, 10
	{
		if(nsecs>0)
			nsecs--, dots .= "."
		else
			break
	}
	
	if(dots)
		linemsg := dots "`r`n" linemsg

	FileAppend, % linemsg "`r`n", *
}

; prints("Ctrl+F1 to output one message with timestamp.")

; ^F1:: prints(A_Now)
