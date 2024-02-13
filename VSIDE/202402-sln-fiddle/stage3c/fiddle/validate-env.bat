@echo off
set batfilenam=%~n0%~x0
set batdir=%~dp0
set batdir=%batdir:~0,-1%

echo ====Running "%batdir%\%batfilenam%"

if not defined SPAMSDK (
	echo [ERROR] User Env-var SPAMSDK not defined.
	exit /b 4
)

if not exist "%SPAMSDK%\include\spam.h" (
	echo [ERROR] SPAMSDK directory invalid, because "%SPAMSDK%\include\spam.h" does not exist.
	exit /b 4
)

exit /b 0
