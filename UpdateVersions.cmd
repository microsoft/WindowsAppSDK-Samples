@echo off

powershell -ExecutionPolicy Unrestricted -NoLogo -NoProfile -File %~dp0\UpdateVersions.ps1 %*

exit /b %ERRORLEVEL%