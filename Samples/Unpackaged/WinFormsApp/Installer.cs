using System.Runtime.InteropServices;

namespace WinFormsApp
{
    // This class is responsible for running the installer for the Windows App Runtime.
    // It uses P/Invoke to call native Windows API functions to create a process with specific attributes.
    // The installer is expected to be located in the user's Downloads folder or in the same directory as the module.
    // The installer path can be customized by providing a different path.
    public static class Installer
    {
        [DllImport("kernel32.dll", SetLastError = true)]
        static extern bool InitializeProcThreadAttributeList(IntPtr lpAttributeList, int dwAttributeCount, int dwFlags, ref IntPtr lpSize);

        [DllImport("kernel32.dll", SetLastError = true)]
        static extern bool UpdateProcThreadAttribute(IntPtr lpAttributeList, uint dwFlags, IntPtr Attribute, IntPtr lpValue, IntPtr cbSize, IntPtr lpPreviousValue, IntPtr lpReturnSize);

        [DllImport("kernel32.dll", SetLastError = true)]
        static extern void DeleteProcThreadAttributeList(IntPtr lpAttributeList);

        [DllImport("kernel32.dll", SetLastError = true)]
        static extern bool CreateProcess(string? lpApplicationName, string lpCommandLine, IntPtr lpProcessAttributes, IntPtr lpThreadAttributes, bool bInheritHandles, uint dwCreationFlags, IntPtr lpEnvironment, string? lpCurrentDirectory, ref STARTUPINFOEX lpStartupInfo, out PROCESS_INFORMATION lpProcessInformation);

        [DllImport("kernel32.dll", SetLastError = true)]
        static extern bool GetExitCodeProcess(IntPtr hProcess, out uint lpExitCode);

        [DllImport("kernel32.dll", SetLastError = true)]
        static extern uint WaitForSingleObject(IntPtr hHandle, uint dwMilliseconds);

        [StructLayout(LayoutKind.Sequential)]
        struct STARTUPINFOEX
        {
            public STARTUPINFO StartupInfo;
            public IntPtr lpAttributeList;
        }

        [StructLayout(LayoutKind.Sequential)]
        struct STARTUPINFO
        {
            public int cb;
            public string lpReserved;
            public string lpDesktop;
            public string lpTitle;
            public int dwX;
            public int dwY;
            public int dwXSize;
            public int dwYSize;
            public int dwXCountChars;
            public int dwYCountChars;
            public int dwFillAttribute;
            public int dwFlags;
            public short wShowWindow;
            public short cbReserved2;
            public IntPtr lpReserved2;
            public IntPtr hStdInput;
            public IntPtr hStdOutput;
            public IntPtr hStdError;
        }

        [StructLayout(LayoutKind.Sequential)]
        struct PROCESS_INFORMATION
        {
            public IntPtr hProcess;
            public IntPtr hThread;
            public uint dwProcessId;
            public uint dwThreadId;
        }

        const uint EXTENDED_STARTUPINFO_PRESENT = 0x00080000;
        const uint PROC_THREAD_ATTRIBUTE_DESKTOP_APP_POLICY = 0x00020007;
        const uint PROCESS_CREATION_DESKTOP_APP_BREAKAWAY_ENABLE_PROCESS_TREE = 0x00000001;

        public static void RunInstaller(string path)
        {
            try
            {
                string exePath = GenerateInstallerPath(path);

                IntPtr attributeListSize = IntPtr.Zero;
                int attributeCount = 1;

                if (!InitializeProcThreadAttributeList(IntPtr.Zero, attributeCount, 0, ref attributeListSize))
                {
                    int lastError = Marshal.GetLastWin32Error();
                    if (lastError != 122) // ERROR_INSUFFICIENT_BUFFER
                    {
                        throw new Exception($"Unexpected error: {lastError}");
                    }
                }

                IntPtr attributeListBuffer = Marshal.AllocHGlobal(attributeListSize);
                IntPtr attributeList = attributeListBuffer;

                if (!InitializeProcThreadAttributeList(attributeList, attributeCount, 0, ref attributeListSize))
                {
                    throw new Exception($"Failed to initialize attribute list: {Marshal.GetLastWin32Error()}");
                }

                try
                {
                    IntPtr policy = Marshal.AllocHGlobal(sizeof(uint));
                    Marshal.WriteInt32(policy, (int)PROCESS_CREATION_DESKTOP_APP_BREAKAWAY_ENABLE_PROCESS_TREE);

                    if (!UpdateProcThreadAttribute(attributeList, 0, (IntPtr)PROC_THREAD_ATTRIBUTE_DESKTOP_APP_POLICY, policy, (IntPtr)sizeof(uint), IntPtr.Zero, IntPtr.Zero))
                    {
                        throw new Exception($"Failed to update attribute: {Marshal.GetLastWin32Error()}");
                    }

                    STARTUPINFOEX info = new STARTUPINFOEX();
                    info.StartupInfo.cb = Marshal.SizeOf(info);
                    info.lpAttributeList = attributeList;

                    PROCESS_INFORMATION processInfo;
                    if (!CreateProcess(null, exePath, IntPtr.Zero, IntPtr.Zero, false, EXTENDED_STARTUPINFO_PRESENT, IntPtr.Zero, null, ref info, out processInfo))
                    {
                        throw new Exception($"Failed to create process: {Marshal.GetLastWin32Error()}");
                    }

                    uint waitResult = WaitForSingleObject(processInfo.hProcess, uint.MaxValue);
                    if (waitResult != 0)
                    {
                        throw new Exception($"Failed to wait for process: {Marshal.GetLastWin32Error()}");
                    }

                    if (!GetExitCodeProcess(processInfo.hProcess, out uint processExitCode))
                    {
                        throw new Exception($"Failed to get exit code: {Marshal.GetLastWin32Error()}");
                    }

                    if (processExitCode != 0)
                    {
                        throw new Exception($"Installer exited with code: 0x{processExitCode:X}");
                    }
                }
                finally
                {
                    DeleteProcThreadAttributeList(attributeList);
                    Marshal.FreeHGlobal(attributeListBuffer);
                }
            }
            catch (Exception ex)
            {
                throw new InvalidOperationException("An error occurred while running the installer to add packages.", ex);
            }
        }

        private static string GenerateInstallerPath(string path)
        {
            string installerPath;
            if (string.IsNullOrEmpty(path))
            {
                // Assume the installer is in the same directory as module 
                string module = System.Reflection.Assembly.GetExecutingAssembly().Location;
                string? dirPath = Path.GetDirectoryName(module);
                if (dirPath == null)
                {
                    throw new InvalidOperationException("Failed to get the directory name of the executing assembly.");
                }
                installerPath = Path.Combine(dirPath, "windowsappruntimeinstaller.exe");
            }
            else
            {
                installerPath = Environment.ExpandEnvironmentVariables(path);
            }
            return installerPath;
        }
    }
}
