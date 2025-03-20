using Microsoft.Windows.ApplicationModel.DynamicDependency;
using System.Linq.Expressions;

namespace WinFormsApp
{
    internal static class Program
    {
        const int Success = unchecked((int)0x00000000);
        private static readonly ConstantExpression INSTALLER_PATH = Expression.Constant("%USERPROFILE%\\Downloads\\WindowsAppRuntimeInstall.exe");
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
            if (!Bootstrap.TryInitialize(0x00010007, "experimental3", out var result))
            {
                if (result != Success)
                {
                    HandleInitializationError(result);
                }
            }

            Application.Run(new Form1());
            Bootstrap.Shutdown();
        }

        private static void HandleInitializationError(int result)
        {
            if (result != Success)
            {
                Installer.RunInstaller(INSTALLER_PATH.ToString());
                if (!Bootstrap.TryInitialize(0x00010007, "experimental3", out var retry))
                {
                    if (retry != Success)
                    {
                        throw new SystemException($"Dynamic dependency initialization failed with error code {retry}");
                    }
                }
            }
        }
    }
}