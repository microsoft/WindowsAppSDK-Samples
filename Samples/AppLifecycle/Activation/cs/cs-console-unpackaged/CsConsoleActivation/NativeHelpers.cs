// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

using System;
using System.Runtime.InteropServices;
using System.Text;

namespace CsConsoleActivation
{
    public class NativeHelpers
    {
        public const int MAX_PATH = 260;

        [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Unicode)]
        [PreserveSig]
        public static extern uint GetModuleFileName
        (
            [In] IntPtr hModule,
            [Out] StringBuilder lpFilename,
            [In][MarshalAs(UnmanagedType.U4)] int nSize
        );
    }
}
