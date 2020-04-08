@echo off
REM https://ss64.com/nt/syntax-functions.html
REM https://stackoverflow.com/questions/1687014/how-do-i-compare-timestamps-of-files-in-a-batch-script


REM Demo: Create a.txt bb.txt c.txt with increasing timestamp, see the compare result
echo Creating a.txt
echo a > a.txt
call :DelaySeconds 1

echo Creating b.txt
echo b > b.txt
call :DelaySeconds 1

echo Creating c.txt
echo c > c.txt


set file1=a.txt
set file2=b.txt
call :IsFile1Newer %file1% %file2%
echo Is "%file1%" newer than "%file2%" : %_ret%


set file1=c.txt
set file2=b.txt
call :IsFile1Newer %file1% %file2%
echo Is "%file1%" newer than "%file2%" : %_ret%

goto :eof

REM Function IsFile1Newer(), check %_ret% for result, result is "yes" or "no".
REM Limitation: input file MUST NOT have directory prefix.
:IsFile1Newer
SETLOCAL
  SET _file1=%1
  SET _file2=%2
  if not exist "%_file2%" (
    set _result=yes
    goto :END_IsFileNewer
  )

  for /F "tokens=*" %%a in ('forfiles /m %_file1% /c "cmd /c echo @fdate @ftime"') do set DATE1=%%a
  for /F "tokens=*" %%a in ('forfiles /m %_file2% /c "cmd /c echo @fdate @ftime"') do set DATE2=%%a
  IF "%DATE1%"=="%DATE2%" (
    set _result=no
    goto :END_IsFileNewer
  )

  FOR /F %%i IN ('DIR /B /O:D %_file1% %_file2%') DO SET _newer=%%i
  if "%_newer%"=="%_file1%" (
    set _result=yes
    goto :END_IsFileNewer
  )
:END_IsFileNewer  
ENDLOCAL & SET _ret=%_result%
exit /b

:DelaySeconds
  echo Delay %1 seconds...
  ping 127.0.0.1 -n %1 -w 1000 > nul
  ping 127.0.0.1 -n 2 -w 1000 > nul
exit /b
