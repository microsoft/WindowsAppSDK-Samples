using System.Reflection;
using System.Runtime.InteropServices;
using System.Windows;
using System.IO;
using Windows.Foundation;
using Windows.Management.Deployment;
using Windows.ApplicationModel;

namespace WindowsAISampleForWPF;

/// <summary>
/// Interaction logic for App.xaml
/// </summary>
public partial class App : Application
{
    // All the interpolated native calls and DLLImports go here
    private static class NativeMethods
    {
        [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
        public static extern int GetCurrentPackageFullName(ref int packageFullNameLength, [Out] char[] packageFullName);
        
        public enum ActivateOptions
        {
            None = 0x00000000,  // No flags set
            DesignMode = 0x00000001,  // The application is being activated for design mode, and thus will not be able to
                                      // to create an immersive window. Window creation must be done by design tools which
                                      // load the necessary components by communicating with a designer-specified service on
                                      // the site chain established on the activation manager.  The splash screen normally
                                      // shown when an application is activated will also not appear.  Most activations
                                      // will not use this flag.
            NoErrorUI = 0x00000002,   // Do not show an error dialog if the app fails to activate.                                
            NoSplashScreen = 0x00000004,  // Do not show the splash screen when activating the app.
        }

        public const string CLSID_ApplicationActivationManager_String = "45ba127d-10a8-46ea-8ab7-56ea9078943c";
        public const string CLSID_IApplicationActivationManager_String = "2e941141-7f97-4756-ba1d-9decde894a3d";

        public static readonly Guid CLSID_ApplicationActivationManager = new Guid(CLSID_ApplicationActivationManager_String);
        public static readonly Guid CLSID_IApplicationActivationManager = new Guid(CLSID_IApplicationActivationManager_String);

        [ComImport, Guid(CLSID_IApplicationActivationManager_String), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
        public interface IApplicationActivationManager
        {
            // Activates the specified immersive application for the "Launch" contract, passing the provided arguments
            // string into the application.  Callers can obtain the process Id of the application instance fulfilling this contract.
            IntPtr ActivateApplication([In] String appUserModelId, [In] String arguments, [In] ActivateOptions options, [Out] out UInt32 processId);
            IntPtr ActivateForFile([In] String appUserModelId, [In] IntPtr /*IShellItemArray* */ itemArray, [In] String verb, [Out] out UInt32 processId);
            IntPtr ActivateForProtocol([In] String appUserModelId, [In] IntPtr /* IShellItemArray* */itemArray, [Out] out UInt32 processId);
        }

        [DllImport("user32.dll", SetLastError = true)]
        public static extern bool SetForegroundWindow(IntPtr hWnd);

        [DllImport("ole32.dll", CharSet = CharSet.Unicode, ExactSpelling = true, PreserveSig = false)]
        public static extern UInt32 CoCreateInstance(
            [In, MarshalAs(UnmanagedType.LPStruct)] Guid rclsid,
            IntPtr pUnkOuter,
            CLSCTX dwClsContext,
            [In, MarshalAs(UnmanagedType.LPStruct)] Guid riid,
            [MarshalAs(UnmanagedType.IUnknown)] out object rReturnedComObject);

        [Flags]
        public enum CLSCTX : uint
        {
            CLSCTX_INPROC_SERVER = 0x1,
            CLSCTX_INPROC_HANDLER = 0x2,
            CLSCTX_LOCAL_SERVER = 0x4,
            CLSCTX_INPROC_SERVER16 = 0x8,
            CLSCTX_REMOTE_SERVER = 0x10,
            CLSCTX_INPROC_HANDLER16 = 0x20,
            CLSCTX_RESERVED1 = 0x40,
            CLSCTX_RESERVED2 = 0x80,
            CLSCTX_RESERVED3 = 0x100,
            CLSCTX_RESERVED4 = 0x200,
            CLSCTX_NO_CODE_DOWNLOAD = 0x400,
            CLSCTX_RESERVED5 = 0x800,
            CLSCTX_NO_CUSTOM_MARSHAL = 0x1000,
            CLSCTX_ENABLE_CODE_DOWNLOAD = 0x2000,
            CLSCTX_NO_FAILURE_LOG = 0x4000,
            CLSCTX_DISABLE_AAA = 0x8000,
            CLSCTX_ENABLE_AAA = 0x10000,
            CLSCTX_FROM_DEFAULT_CONTEXT = 0x20000,
            CLSCTX_ACTIVATE_32_BIT_SERVER = 0x40000,
            CLSCTX_ACTIVATE_64_BIT_SERVER = 0x80000,
            CLSCTX_INPROC = CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER,
            CLSCTX_SERVER = CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER | CLSCTX_REMOTE_SERVER,
            CLSCTX_ALL = CLSCTX_SERVER | CLSCTX_INPROC_HANDLER
        }
    }

    public App()
    {
    }

    // Originally StartupUri="MainWindow.xaml" was used instead of Startup="OnStartUp" in
    // the App.xaml Application properties and the startup code lived in MainWindow.xaml.cs
    // But this design presents a flaw where the unpackaged process will show itself before
    // the package process is able to activate. 
    //
    // This could be mitigated using this.hide on the MainWindows startup but the unpackaged
    // process still showed itself for a split second.
    //
    // Using OnStartUp and intializating the MainWindow once we know we are in the packaged process
    // proved to be a proper solution to avoid this issue.
    private void OnStartUp(Object sender, StartupEventArgs e)
    {
        _ = RestartWithIdentityIfNecessaryAsync();
        InitializeComponent();
    }

    private async Task RestartWithIdentityIfNecessaryAsync()
    {
        // If we are in the packaged process, then present the MainWindow
        if (IsPackagedProcess())
        {
            var mainWindow = new MainWindow();
            MainWindow.Show(); // Show the main window
            return;
        }

        // If we are still unpackaged process, then register the MSIX and ActivateApplication
        Task install = RegisterSparsePackage();
        await install;
        RunWithIdentity();
        Environment.Exit(0);
    }

    private async Task RegisterSparsePackage()
    {
        // We expect the MSIX to be in the same directory as the exe. 
        string? exePath = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
        if (exePath == null)
        {
            throw new InvalidOperationException("Could not determine the directory of the executing assembly.");
        }
        
        string externalLocation = exePath;
        string sparsePkgPath = exePath + "\\WindowsAISampleForWPFSparse.msix";

        Uri externalUri = new Uri(externalLocation);
        Uri packageUri = new Uri(sparsePkgPath);

        PackageManager packageManager = new PackageManager();
        int count = packageManager.FindPackagesForUserWithPackageTypes("", "WindowsAISampleForWPFSparse_k0t3h69cz9sxw", PackageTypes.Main).ToList<Package>().Count;
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

    private void RunWithIdentity()
    {
        // Activating the packaged process
        // We should already know our AUMID which depends on the AppxManifest we defined so this can be hardcoded here. 
        string appUserModelId = "WindowsAISampleForWPFSparse_k0t3h69cz9sxw!WindowsAISampleForWPFSparsePkg";
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
    }

    private static bool IsPackagedProcess()
    {
        int length = 0;
        int result = NativeMethods.GetCurrentPackageFullName(ref length, null!);
        if (result == 15700) // APPMODEL_ERROR_NO_PACKAGE
        {
            return false;
        }
        char[] packageFullName = new char[length];
        result = NativeMethods.GetCurrentPackageFullName(ref length, packageFullName);
        return result == 0;
    }
}
