@echo off
if "%1"=="/?" goto :usage
if "%1"=="-?" goto :usage
if "%VSINSTALLDIR%" == "" goto :usage

set platform=%1
set configuration=%2
set sample_filter=%3\

if "%platform%"=="" set platform=x64
if "%configuration%"=="" set configuration=Release

if not exist ".\.nuget" mkdir ".\.nuget"
if not exist ".\.nuget\nuget.exe" powershell -Command "Invoke-WebRequest https://dist.nuget.org/win-x86-commandline/latest/nuget.exe -OutFile .\.nuget\nuget.exe"

for /f "delims=" %%D in ('dir /s/b samples\%sample_filter%*.sln') do (
    call .nuget\nuget.exe restore "%%D" -configfile Samples\nuget.config
    call msbuild /p:platform=%platform% /p:configuration=%configuration% "%%D"

    if ERRORLEVEL 1 goto :eof
)
goto :eof

:usage
echo Usage:
echo     This script should be run under a Visual Studio Developer Command Prompt.
echo.
echo     build.cmd [Platform] [Configuration] [Sample]
echo.
echo     [Platform] Either x86, x64, or arm64.  Default is x64.
echo     [Configuration] Either Debug or Release.  Default is Release.
echo     [Sample] The sample folder under Samples to build.  If none specified, all samples are built.
echo.
echo     If no parameters are specified, all samples are built for x64 Release.

exit /b /1