@rem WinDbgX -c "$<dbg-options.txt" __output\windows-vs-x64\ResNetConsoleDesktop\Debug\ResNetConsoleDesktop.exe

@rem WinDbgX -c "sxe ld;sxe ud;g" __output\windows-vs-x64\ResNetConsoleDesktop\Debug\ResNetConsoleDesktop.exe

WinDbgX -c "g" __output\windows-vs-x64\ResNetConsoleDesktop\Debug\ResNetConsoleDesktop.exe
