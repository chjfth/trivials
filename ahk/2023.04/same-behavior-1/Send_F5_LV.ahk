
Numpad0::foo()
foo()
{
	SendLevel 1
	Send {F5}
}

#InputLevel 0
F5::MsgBox, "F5 pressed"

; F5 hotkey is triggered, and F5 is NOT passed to target process.
