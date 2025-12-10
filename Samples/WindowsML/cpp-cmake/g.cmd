@rem WinDbgX -c "$<dbg-options.txt" __output\windows-vs-x64\ResNetConsoleDesktop\Debug\ResNetConsoleDesktop.exe

WinDbgX -c "sxe ld;sxe ud;g" __output\windows-vs-x64\ResNetConsoleDesktop\Debug\ResNetConsoleDesktop.exe
