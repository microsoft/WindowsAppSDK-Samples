using Microsoft.Windows.ApplicationModel.DynamicDependency;

namespace WinFormsApp
{
    internal static class Program
    {
        /// <summary>
        ///  The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            // This sample uses explicit initialization of the Windows App SDK framework package.
            // For explicit initialization, call Bootstrap.Initialize() or Bootstrap.TryInitialize to load the Windows App SDK framework package
            // and remove WindowsPackageType=None in project file
            // Learn more at: https://learn.microsoft.com/windows/apps/windows-app-sdk/tutorial-unpackaged-deployment?tabs=csharp
            const int Success = unchecked((int)0x00000000);
            if (!Bootstrap.TryInitialize(0x00010006, out int result))
            {
                if (result != Success)
                {
                    throw new SystemException($"Dynamic dependency initialization failed with error code {result}");
                }
            }

            Application.Run(new Form1());
            Bootstrap.Shutdown();
        }
    }
}