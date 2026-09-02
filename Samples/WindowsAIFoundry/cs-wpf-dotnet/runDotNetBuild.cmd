@echo off

powershell -ExecutionPolicy Unrestricted -NoLogo -NoProfile -File %~dp0\runDotNetBuild.ps1 %*

exit /b %ERRORLEVEL%