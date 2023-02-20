~F3:: DoF3()
DoF3()
{
	static s_count := 0
	s_count++
	tooltip, % A_ThisFunc " triggered #" s_count
	
	Send {F3}
}
