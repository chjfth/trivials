@REM  == Add resgen.exe to path
PATH=C:\Program Files (x86)\Microsoft SDKs\Windows\v10.0A\bin\NETFX 4.6.2 Tools;%PATH%

@REM == Use resgen.exe to generate .resources file.
@REM == The output file content has nothing to do with output filename.
@REM == But, the filename reflects the namespace of the .resource file which is critical.
resgen.exe Resource1.resx testRes.Resource1.resources
@if ERRORLEVEL 1 (
    echo resgen execution Error!
    exit /b 1
)

@set CSFILES=Form1.cs Form1.Designer.cs Program.cs Resource1.Designer.cs Properties\Settings.Designer.cs Properties\Resources.Designer.cs Properties\AssemblyInfo.cs

@REM == Call csc.exe to compile .cs and link resources.
csc.exe %CSFILES% /resource:testRes.Resource1.resources
@if ERRORLEVEL 1 (
    echo csc execution Error!
    exit /b 1
)

@echo Compile success.
