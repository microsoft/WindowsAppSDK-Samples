
set srcdir=%1

set src=%srcdir%\Microsoft.ui.xaml.dll
set dst=D:\WindowsAppSDK-Samples\Samples\Islands\SimpleIslandApp\cpp-win32-unpackaged\x64\Release\SimpleIslandApp\Microsoft.ui.xaml.dll

copy /y %src% %dst%

symst %srcdir%\Microsoft.ui.xaml.pdb
