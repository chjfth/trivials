b::a

^a:: ttcount()
ttcount()
{
	static s_count := 0
	s_count++
	tooltip, % "Want to block Ctrl+a #" s_count
}
