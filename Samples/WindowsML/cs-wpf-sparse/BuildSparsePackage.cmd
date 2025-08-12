@echo off
setlocal

echo Building WindowsML WPF Sparse Sample...

rem Build the application
dotnet build WindowsMLSampleForWPF.sln -c Release

if %errorlevel% neq 0 (
    echo Build failed!
    exit /b %errorlevel%
)

rem Create sparse package
echo Creating sparse package...
MakeAppx.exe pack /d "WindowsMLSampleForWPF\bin\Release\net9.0-windows10.0.26100.0\win-x64\" /p "WindowsMLSampleForWPF_x64.msix" /o

if %errorlevel% neq 0 (
    echo Package creation failed!
    exit /b %errorlevel%
)

echo Build and packaging completed successfully!
echo Package created: WindowsMLSampleForWPF_x64.msix
