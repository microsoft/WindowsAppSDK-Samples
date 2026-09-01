@echo off
:: Thin wrapper calling PowerShell implementation
set SCRIPT_DIR=%~dp0
set PS_SCRIPT=%SCRIPT_DIR%build.ps1

if "%1"=="/?" goto :usage
if "%1"=="-?" goto :usage

:: Forward all arguments directly; build.ps1 handles defaults/validation
powershell -NoProfile -ExecutionPolicy Bypass -File "%PS_SCRIPT%" %*
exit /b %ERRORLEVEL%

:usage
echo Usage:
echo     build.cmd [Platform] [Configuration] [Sample]
echo.
echo     (Wrapper over build.ps1)
echo     Platform: x86|x64|arm64 (default x64)
echo     Configuration: Debug|Release (default Release)
echo     Sample: Optional sample folder name under Samples
exit /b 1