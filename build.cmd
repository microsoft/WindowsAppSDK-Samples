@echo off
if "%1"=="/?" goto :usage
if "%1"=="-?" goto :usage
if "%VSINSTALLDIR%" == "" goto :usage

setlocal enabledelayedexpansion enableextensions

set BUILDCMDSTARTTIME=%time%

set platform=%1
set configuration=%2
set sample_filter=%3\

if "%platform%"=="" set platform=x64
if "%configuration%"=="" set configuration=Release

if not exist ".\.nuget" mkdir ".\.nuget"
if not exist ".\.nuget\nuget.exe" powershell -Command "Invoke-WebRequest https://dist.nuget.org/win-x86-commandline/latest/nuget.exe -OutFile .\.nuget\nuget.exe"

set NUGET_RESTORE_MSBUILD_ARGS=/p:PublishReadyToRun=true

for /f "delims=" %%D in ('dir /s/b samples\%sample_filter%*.sln') do (
    call .nuget\nuget.exe restore "%%D" -configfile Samples\nuget.config -PackagesDirectory %~dp0packages
    call msbuild /warnaserror /p:platform=%platform% /p:configuration=%configuration% /p:NugetPackageDirectory=%~dp0packages /bl:"%%~nD.binlog" "%%D"

    if ERRORLEVEL 1 goto :eof
)

:showDurationAndExit
set BUILDCMDENDTIME=%time%
:: Note: The '1's in this line are to convert a value like "08" to "108", since numbers which
::       begin with '0' are interpreted as octal, which makes "08" and "09" invalid. Adding the
::       '1's effectively adds 100 to both sides of the subtraction, avoiding this issue.
::       Hours has a leading space instead of 0, so the '1's trick isn't used on that one.
set /a BUILDDURATION_HRS= %BUILDCMDENDTIME:~0,2%- %BUILDCMDSTARTTIME:~0,2%
set /a BUILDDURATION_MIN=1%BUILDCMDENDTIME:~3,2%-1%BUILDCMDSTARTTIME:~3,2%
set /a BUILDDURATION_SEC=1%BUILDCMDENDTIME:~6,2%-1%BUILDCMDSTARTTIME:~6,2%
set /a BUILDDURATION_HSC=1%BUILDCMDENDTIME:~9,2%-1%BUILDCMDSTARTTIME:~9,2%
if %BUILDDURATION_HSC% lss 0 (
    set /a BUILDDURATION_HSC=!BUILDDURATION_HSC!+100
    set /a BUILDDURATION_SEC=!BUILDDURATION_SEC!-1
)
if %BUILDDURATION_SEC% lss 0 (
    set /a BUILDDURATION_SEC=!BUILDDURATION_SEC!+60
    set /a BUILDDURATION_MIN=!BUILDDURATION_MIN!-1
)
if %BUILDDURATION_MIN% lss 0 (
    set /a BUILDDURATION_MIN=!BUILDDURATION_MIN!+60
    set /a BUILDDURATION_HRS=!BUILDDURATION_HRS!-1
)
if %BUILDDURATION_HRS% lss 0 (
    set /a BUILDDURATION_HRS=!BUILDDURATION_HRS!+24
)
:: Add a '0' at the start to ensure at least two digits. The output will then just
:: show the last two digits for each.
set BUILDDURATION_HRS=0%BUILDDURATION_HRS%
set BUILDDURATION_MIN=0%BUILDDURATION_MIN%
set BUILDDURATION_SEC=0%BUILDDURATION_SEC%
set BUILDDURATION_HSC=0%BUILDDURATION_HSC%
echo ---
echo Start time: %BUILDCMDSTARTTIME%. End time: %BUILDCMDENDTIME%
echo    Elapsed: %BUILDDURATION_HRS:~-2%:%BUILDDURATION_MIN:~-2%:%BUILDDURATION_SEC:~-2%.%BUILDDURATION_HSC:~-2%
endlocal

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