
using System.Diagnostics;
using System.Reflection;
using Windows.Foundation;
using Windows.Management.Deployment;

namespace WindowsCopilotRuntimeSample
{
    internal static class Program
    {
        /// <summary>
        ///  The main entry point for the Winforms application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            LaunchWithIdentity();
        }

        private static async void LaunchWithIdentity()
        {
            try
            {
                // To customize application configuration such as set high DPI settings or default font,
                // see https://aka.ms/applicationconfiguration.
                await RestartWithIdentityIfNecessary();
                Application.Run(new MainForm(string.Empty));
            }
            catch (Exception ex)
            {
                // Log the error to the debug output
                Debug.WriteLine($"Application exited with an error: {ex.Message}");
                Debug.WriteLine($"Stack Trace: {ex.StackTrace}");
            }
        }

        private static async Task RestartWithIdentityIfNecessary()
        {
            if (IsPackagedProcess())
            {
                // We are already in a packaged process, so we can just run the application normally
                return;
            }

            // If we are still unpackaged process, then register the MSIX and ActivateApplication
            Task install = RegisterSparsePackage();
            await install;
            RunWithIdentity();
            Environment.Exit(0);
        }

        private static async Task RegisterSparsePackage()
        {
            // We expect the MSIX to be in the same directory as the exe. 
            string? exePath = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
            string? externalLocation = exePath;
            string sparsePkgPath = exePath + "\\WCRforWinformsSparse.msix";

            Uri externalUri = new Uri(externalLocation!);
            Uri packageUri = new Uri(sparsePkgPath);

            PackageManager packageManager = new PackageManager();
            int count = packageManager.FindPackagesForUserWithPackageTypes("", "WCRforWinformsSparse_k0t3h69cz9sxw", PackageTypes.Main).Count();
            if (count == 0)
            {
                //Declare use of an external location
                var options = new AddPackageOptions();
                options.ExternalLocationUri = externalUri;

                var deploymentOperation = packageManager.AddPackageByUriAsync(packageUri, options);
                var deploymentResult = await deploymentOperation;

                if (deploymentOperation.Status == AsyncStatus.Completed)
                {
                    return;
                }
                else
                {
                    throw new Exception("Package did not register");
                }
            }
        }

        private static void RunWithIdentity()
        {
            // Activating the packaged process
            // We should already know our AUMID which depends on the AppxManifest we defined so this can be hardcoded here. 
            string appUserModelId = "WCRforWinformsSparse_k0t3h69cz9sxw!WCRforWinformsSparsePkg";
            if (NativeMethods.CoCreateInstance(
                NativeMethods.CLSID_ApplicationActivationManager,
                IntPtr.Zero,
                NativeMethods.CLSCTX.CLSCTX_LOCAL_SERVER,
                NativeMethods.CLSID_IApplicationActivationManager,
                out object applicationActivationManagerAsObject) != 0)
            {
                throw new Exception("Failed to create ApplicationActivationManager!");
            }
            var applicationActivationManager = (NativeMethods.IApplicationActivationManager)applicationActivationManagerAsObject;
            applicationActivationManager.ActivateApplication(appUserModelId, string.Empty, NativeMethods.ActivateOptions.None, out uint processId);
            if (processId == 0)
            {
                throw new Exception("Failed to activate the application!");
            }
        }

        private static bool IsPackagedProcess()
        {
            int length = 0;
            int result = NativeMethods.GetCurrentPackageFullName(ref length, Array.Empty<char>());
            if (result == 15700) // APPMODEL_ERROR_NO_PACKAGE
            {
                return false;
            }
            char[] packageFullName = new char[length];
            result = NativeMethods.GetCurrentPackageFullName(ref length, packageFullName);
            return result == 0;
        }
    }
}