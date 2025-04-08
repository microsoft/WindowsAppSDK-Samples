﻿using System.Text;
using System.Threading.Tasks;
using System.Reflection;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Drawing.Imaging;
using Microsoft.Graphics.Imaging;
using Microsoft.Windows.AI.Generative;
using Microsoft.Windows.Vision;
using Windows.Graphics.Imaging;
using Windows.Storage;
using Windows.Management.Deployment;
using Windows.ApplicationModel;
using Windows.ApplicationModel.Activation;
using System.Runtime.InteropServices;
using Windows.ApplicationModel.Activation;
using System.Diagnostics;
using Windows.Foundation;


namespace WCRforWPF;

/// <summary>
/// Interaction logic for MainWindow.xaml
/// </summary>
public partial class MainWindow : Window
{
    [DllImport("kernel32.dll", SetLastError = true)]
    private static extern IntPtr GetCurrentProcess();

    [DllImport("kernel32.dll", SetLastError = true)]
    private static extern uint GetPackageFamilyName(IntPtr hProcess, ref uint packageFamilyNameLength, [Out] char[] packageFamilyName);

    [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    private static extern int GetCurrentPackageFullName(ref int packageFullNameLength, [Out] char[] packageFullName);

    private static class NativeMethods
    {
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

        [DllImport("api-ms-win-ntuser-ie-window-l1-1-0.dll", SetLastError = true)]
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


    public MainWindow()
    {
        RestartWithIdentityIfNecessary();
        InitializeComponent();
    }

    private void RestartWithIdentityIfNecessary()
    {
        if (IsPackagedProcess())
        {
            return;
        }
        InstallIfNecesary();
        RunWithIdentity();
        Environment.Exit(123);
        
    }
    private void InstallIfNecesary()
    {

        string exePath = Assembly.GetExecutingAssembly().Location;

        //TODO - update the value of externalLocation to match the output location of your VS Build binaries and the value of 
        //sparsePkgPath to match the path to your signed Sparse Package (.msix). 
        //Note that these values cannot be relative paths and must be complete paths
        string externalLocation = exePath;
        string sparsePkgPath = exePath + "\\WCRforWPF.msix";
        registerSparsePackage(externalLocation, sparsePkgPath);
    }

    private void RunWithIdentity()
    {
        string appUserModelId = "WCRforWPF_8wekyb3d8bbwe!WCRforWPFPkg"; // Replace with your AUMID
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
        applicationActivationManager.ActivateApplication(appUserModelId, null, NativeMethods.ActivateOptions.None, out uint processId);


        // Wait for the process to exit
        try
        {
            Process process = Process.GetProcessById((int)processId);
            process.WaitForExit();
            Console.WriteLine("Application has exited.");
        }
        catch (ArgumentException ex)
        {
            Console.WriteLine($"Process with ID {processId} not found: {ex.Message}");
        }
    }

    private static async void registerSparsePackage(string externalLocation, string sparsePkgPath)
    {  
        Uri externalUri = new Uri(externalLocation);
        Uri packageUri = new Uri(sparsePkgPath);

        Console.WriteLine("exe Location {0}", externalLocation);
        Console.WriteLine("msix Address {0}", sparsePkgPath);

        Console.WriteLine("  exe Uri {0}", externalUri);
        Console.WriteLine("  msix Uri {0}", packageUri);

        PackageManager packageManager = new PackageManager();


        if (packageManager.FindPackagesForUserWithPackageTypes("", "WCRforWPF_8wekyb3d8bbwe", PackageTypes.Main).ToList<Package>().Count > 0)
        {
            //Declare use of an external location
            var options = new AddPackageOptions();
            options.ExternalLocationUri = externalUri;

            var deploymentOperation = packageManager.AddPackageByUriAsync(packageUri, options);
            var deploymentResult = await deploymentOperation;

            if (deploymentOperation.Status == AsyncStatus.Completed)
            {
                Console.WriteLine("OK");
            }
            else
            {
                Console.WriteLine("Error:{}",
                    deploymentResult.ErrorText);
            }
        }
        
    }

    private async void SelectFile_Click(object sender, RoutedEventArgs e)
    {
        _currentImage = null;
        this.Description.Text = "(picking a file)";
        this.FileContent.Text = "(picking a file)";

        var fileDialog = new Microsoft.Win32.OpenFileDialog();
        fileDialog.DefaultExt = ".jpg";
        fileDialog.Filter = "Image files (*.jpg, *.jpeg, *.png)|*.jpg;*.jpeg;*.png";
        if (fileDialog.ShowDialog() == true)
        {
            this.FilePath.Text = fileDialog.FileName;
            this.ProcessFile.IsEnabled = true;
            _currentImage = await GetImageBufferFromFile(fileDialog.FileName);
            var source = new BitmapImage(new Uri(fileDialog.FileName));
            this.InputImage.Source = source;
        }
        uint length = 0;
        IntPtr processHandle = GetCurrentProcess();
        uint result = GetPackageFamilyName(processHandle, ref length, null);

        if (IsPackagedProcess())
        {
            Console.WriteLine("IsPackagedProcess");
            this.Description.Text = "IsPackagedProcess";
            this.FileContent.Text = "IsPackagedProcess";
        }
        else
        {
            this.Description.Text = "No package identity.";
            this.FileContent.Text = "No package identity.";
            Console.WriteLine("No package identity.");
        }
    }

    private void PasteImageContent(object sender, RoutedEventArgs e)
    {
        this.Description.Text = "(pasting)";
        this.FileContent.Text = "(pasting)";
        var clipboard = System.Windows.Clipboard.GetDataObject();
        if (clipboard == null)
        {
            this.FileContent.Text = "Can't open the clipboard, try again";
            return;
        }

        var bitmapContent = clipboard.GetData(DataFormats.Bitmap, true) as BitmapImage;
        if (bitmapContent == null)
        {
            this.FileContent.Text = "That wasn't a bitmap, try again";
            return;
        }
    }

    private ImageDescriptionGenerator? _ig;
    private ImageBuffer? _currentImage;

    private async Task<ImageBuffer> GetImageBufferFromFile(string path)
    {
        var storageFile = await StorageFile.GetFileFromPathAsync(path);
        var inputStream = await storageFile.OpenAsync(FileAccessMode.Read);
        var decoder = await Windows.Graphics.Imaging.BitmapDecoder.CreateAsync(inputStream);
        var frame = await decoder.GetFrameAsync(0);
        var softwareBitmap = await frame.GetSoftwareBitmapAsync();
        return ImageBuffer.CreateCopyFromBitmap(softwareBitmap);
    }

    private async void ProcessFile_Click(object sender, RoutedEventArgs e)
    {
        this.FileContent.Text = "Extracting text...";
        this.Description.Text = "(waiting)";

        // First, extract text from the image using the OCR model
        var ocr = await EnsureTextRecognizerAsync();
        if (ocr == null)
        {
            this.FileContent.Text = "No OCR model available";
            return;
        }

        // Use the current image selected
        if (_currentImage == null)
        {
            this.FileContent.Text = "No image selected, paste or click the selector";
            return;
        }

        var options = new TextRecognizerOptions();
        options.MaxLineCount = 50;
        this.FileContent.Text = "Extracting text...";
        var imageText = await ocr.RecognizeTextFromImageAsync(_currentImage, options);

        this.FileContent.Text = "";
        foreach (var line in imageText.Lines)
        {
            this.FileContent.Text += line.Text + "\n";
        }

        // Use the image descriptor and see what it thinks of the image
        var ig = await EnsureDescriptionGenerator();
        if (ig == null)
        {
            this.Description.Text = "No image descriptor available";
            return;
        }

        this.Description.Text = "Generating description...";
        var imageDescription = await ig.DescribeAsync(_currentImage, ImageDescriptionScenario.Accessibility);
        this.Description.Text = imageDescription.Response;
    }

    private TextRecognizer? _ocr;

    private async Task<TextRecognizer?> EnsureTextRecognizerAsync()
    {
        if (_ocr == null)
        {
            this.FileContent.Text = "Loading OCR model...";
            try
            {
                if (!TextRecognizer.IsAvailable())
                {
                    this.FileContent.Text = "Making the OCR model available...";
                    var op = await TextRecognizer.MakeAvailableAsync();
                }
                this.FileContent.Text = "Loading OCR model...";
                _ocr = await TextRecognizer.CreateAsync();
            }
            catch (Exception ex)
            {
                this.FileContent.Text = $"Caught exception {ex} trying to load the OCR model";
            }
        }
        return _ocr;
    }

    private async Task<ImageDescriptionGenerator?> EnsureDescriptionGenerator()
    {
        if (_ig == null)
        {
            this.Description.Text = "Loading image description generator...";

            try
            {
                if (!ImageDescriptionGenerator.IsAvailable())
                {
                    this.Description.Text = "Making the descriptor available...";
                    var op = await ImageDescriptionGenerator.MakeAvailableAsync();
                }
                this.Description.Text = "Loading image description generator...";
                _ig = await ImageDescriptionGenerator.CreateAsync();
            }
            catch (Exception ex)
            {
                this.Description.Text = $"Caught exception {ex} trying to load the image description generator";
            }
        }

        return _ig;
    }


    static bool IsPackagedProcess()
    {
        int length = 0;
        int result = GetCurrentPackageFullName(ref length, null);

        if (result == 15700) // APPMODEL_ERROR_NO_PACKAGE
        {
            return false;
        }

        char[] packageFullName = new char[length];
        result = GetCurrentPackageFullName(ref length, packageFullName);

        return result == 0;
    }
}