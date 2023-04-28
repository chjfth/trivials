
Numpad0::foo()
foo()
{
	SendLevel 1
	Send {F5}
}

#InputLevel 0
~F5::MsgBox, "~F5 pressed"

; Behavior: MsgBox pops up, and, target process gets F5 WM_KEYDOWN & WM_KEYUP.
