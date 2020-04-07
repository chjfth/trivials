@echo off
REM https://ss64.com/nt/syntax-functions.html
REM https://stackoverflow.com/questions/1687014/how-do-i-compare-timestamps-of-files-in-a-batch-script


REM Demo: Create a.txt bb.txt c.txt with increasing timestamp, see the compare result
echo Creating a.txt
echo a > a.txt
call :DelaySeconds 3

echo Creating b.txt
echo b > b.txt
call :DelaySeconds 3

echo Creating c.txt
echo c > c.txt

echo 999999999999

set file1=a.txt
set file2=b.txt
call :IsFile1Newer %file1% %file2%
echo Is "%file1%" newer than "%file2%" : %_ret%


set file1=c.txt
set file2=b.txt
call :IsFile1Newer %file1% %file2%
echo Is "%file1%" newer than "%file2%" : %_ret%

goto :eof

:IsFile1Newer
SETLOCAL
  SET _file1=%1
  SET _file2=%2
echo !!! %file1% , %file2%
  FOR %%i IN (%_file1%) DO SET DATE1=%%~ti
  FOR %%i IN (%_file2%) DO SET DATE2=%%~ti
  IF "%DATE1%"=="%DATE2%" (
echo 0000 ["%DATE1%"=="%DATE2%"]
    set _result=no
    goto :END_IsFileNewer
  )

DIR /O:D %_file1% %_file2%
  FOR /F %%i IN ('DIR /B /O:D %_file1% %_file2%') DO SET _newer=%%i
  if "%_newer%"=="%_file1%" (
echo yyyy
    set _result=yes
    goto :END_IsFileNewer
  )
:END_IsFileNewer  
echo ========%_result%
ENDLOCAL & SET _ret=%_result%
exit /b

:DelaySeconds
  echo Delay %1 seconds...
  ping 127.0.0.1 -n %1 -w 1000 > nul
  ping 127.0.0.1 -n 2 -w 1000 > nul
exit /b
