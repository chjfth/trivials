[2022-07-05] Show the tricky side of `__inline` keyword.

Do these experiments in Visual C++ 2019.
=======================================

[CASE 1a] 

	cl main.cpp sq1.cpp sq2.cpp /link /out:mis1a.exe /MAP

Compile success.

> mis1a.exe
prn-square1(0) = 1
prn-square2(0) = 1
main() calling external get_square(0)= 1


[CASE 1b]

	cl main.cpp sq2.cpp sq1.cpp /link /out:mis1b.exe /MAP
	
Compile success.

> mis1b
prn-square1(0) = 2
prn-square2(0) = 2
main() calling external get_square(0)= 2

[CASE 2a]

	cl /O1 main.cpp sq1.cpp sq2.cpp /link /out:mis2a.exe

Compile success.

> mis2a
prn-square1(0) = 1
prn-square2(0) = 2                              <== NOTE THIS!
main() calling external get_square(0)= 1


[CASE 2b]

	cl /O1 main.cpp sq2.cpp sq1.cpp /link /out:mis2b.exe

> mis2b
prn-square1(0) = 1
prn-square2(0) = 2
main() calling external get_square(0)= 2


[CASE 2c]

	cl /Ob1 main.cpp sq1.cpp sq2.cpp /link /out:mis2c.exe

Compile fail.

main.obj : error LNK2019: unresolved external symbol get_square referenced in function main


[CASE 3]

	cl /D REMOVE_INLINE main.cpp sq1.cpp sq2.cpp /link /out:mis3.exe

Compile fail.

sq2.obj : error LNK2005: get_square already defined in sq1.obj
mis3.exe : fatal error LNK1169: one or more multiply defined symbols found


[CASE 4]

	cl /D REMOVE_CALLER main.cpp sq1.cpp sq2.cpp /link /out:mis4.exe

Compile fail.

main.obj : error LNK2019: unresolved external symbol get_square referenced in function main
mis4.exe : fatal error LNK1120: 1 unresolved externals


[CASE 5]

	cl /D REMOVE_CALLER /D DULL_CALLER main.cpp sq1.cpp sq2.cpp /link /out:mis5.exe

Compile success.

> mis5
main() calling external get_square(0)= 1


