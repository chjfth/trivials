; nil: no #InputLevel statement
F1::F9

~F9::DoF9()
DoF9()
{
	static s_count := 0
	s_count++
	tooltip, % "F9 triggered #" s_count
}
