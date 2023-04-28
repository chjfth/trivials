
Numpad0::foo()
foo()
{
	SendLevel 1
	SendInput {F5}
}

#InputLevel 0
~F5::MsgBox, "~F5 pressed"

; F5 hotkey is not triggered; F5 is passed to target process directly.
