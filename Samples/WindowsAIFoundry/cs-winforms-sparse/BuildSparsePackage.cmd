@echo off

powershell -ExecutionPolicy Unrestricted -NoLogo -NoProfile -File %~dp0\BuildSparsePackage.ps1 %*

exit /b %ERRORLEVEL%