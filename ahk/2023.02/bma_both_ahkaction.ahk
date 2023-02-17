b::a

^b:: ttcount()
ttcount()
{
   static s_count := 0
   s_count++
   tooltip, % A_ThisFunc " triggered #" s_count
}

^a:: MsgBox, % "^a ahk-action"
