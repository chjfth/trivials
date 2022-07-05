@REM https://stackoverflow.com/questions/71570625/vs2019-and-no-crt-stdio-inline-how-to-explain-this-weirdo/72852472
@setlocal EnableDelayedExpansion
@set CFLAGS=/D _NO_CRT_STDIO_INLINE

cl /nologo /c /MT %CFLAGS% pwrapper.cpp
@if errorlevel 1 exit /b 4

lib /nologo  /out:pwrapper.lib pwrapper.obj
@if errorlevel 1 exit /b 4

cl /nologo /c /MT  main.cpp
@if errorlevel 1 exit /b 4

link /nologo main.obj pwrapper.lib
@if errorlevel 1 exit /b 4
