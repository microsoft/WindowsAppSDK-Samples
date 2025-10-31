using Microsoft.UI.Xaml;

using System;
using System.Runtime.InteropServices;
using Windows.Win32;
using Windows.Win32.Foundation;
using Windows.Win32.System.Com;
using Windows.Win32.UI.Shell;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace ComparePickerPermissions
{
    /// <summary>
    /// An empty window that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private unsafe void OpenFileButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                StatusText.Text = "Opening file picker using COM API...";
                ResultText.Text = string.Empty;

                // Get the window handle
                var hwnd = WinRT.Interop.WindowNative.GetWindowHandle(this);

                // Create the FileOpenDialog COM object using CsWin32 generated code
                Guid clsid = typeof(FileOpenDialog).GUID;
                Guid iid = typeof(IFileOpenDialog).GUID;

                HRESULT hr = PInvoke.CoCreateInstance(
                    clsid,
                    null,
                    CLSCTX.CLSCTX_INPROC_SERVER,
                    iid,
                    out var ppv);

                if (!hr.Succeeded)
                {
                    StatusText.Text = $"Failed to create COM object. HRESULT: 0x{hr.Value:X8}";
                    return;
                }

                var fileDialog = (IFileOpenDialog)ppv!;

                // Set options for file selection
                fileDialog.SetOptions(FILEOPENDIALOGOPTIONS.FOS_FORCEFILESYSTEM);

                // Show the dialog
                try
                {
                    fileDialog.Show(new HWND(hwnd));

                    // Get the result
                    fileDialog.GetResult(out var resultItem);

                    if (resultItem != null)
                    {
                        // Get the file path
                        resultItem.GetDisplayName(SIGDN.SIGDN_FILESYSPATH, out var pathPtr);

                        string filePath = new string(pathPtr.Value);

                        // Free the memory allocated by COM
                        Marshal.FreeCoTaskMem((IntPtr)pathPtr.Value);

                        StatusText.Text = "File selected successfully!";
                        ResultText.Text = $"Selected file:\n{filePath}";

                        // Release COM objects
                        Marshal.ReleaseComObject(resultItem);
                    }
                    else
                    {
                        StatusText.Text = "No file was selected.";
                        ResultText.Text = string.Empty;
                    }
                }
                catch (COMException comEx) when (comEx.HResult == unchecked((int)0x800704C7))
                {
                    // User cancelled
                    StatusText.Text = "File picker was cancelled.";
                    ResultText.Text = string.Empty;
                }
                catch (COMException comEx)
                {
                    StatusText.Text = $"COM error occurred. HRESULT: 0x{comEx.HResult:X8}";
                    ResultText.Text = $"Message: {comEx.Message}";
                }

                Marshal.ReleaseComObject(fileDialog);
            }
            catch (Exception ex)
            {
                StatusText.Text = "Error occurred!";
                ResultText.Text = $"Exception: {ex.GetType().Name}\nMessage: {ex.Message}\n\nStack trace:\n{ex.StackTrace}";
            }
        }

        /*
        private unsafe void OpenFolderButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                StatusText.Text = "Opening folder picker using COM API...";
                ResultText.Text = string.Empty;

                // Get the window handle
                var hwnd = WinRT.Interop.WindowNative.GetWindowHandle(this);

                // Create the FileOpenDialog COM object using CsWin32 generated code
                Guid clsid = new Guid("DC1C5A9C-E88A-4dde-A5A1-60F82A20AEF7"); // CLSID_FileOpenDialog
                Guid iid = typeof(IFileOpenDialog).GUID;

                HRESULT hr = PInvoke.CoCreateInstance(
                    clsid,
                    null,
                    CLSCTX.CLSCTX_INPROC_SERVER,
                    iid,
                    out var ppv);

                if (!hr.Succeeded)
                {
                    StatusText.Text = $"Failed to create COM object. HRESULT: 0x{hr.Value:X8}";
                    return;
                }

                var fileDialog = (IFileOpenDialog)ppv!;

                // Set options for folder selection
                fileDialog.SetOptions(FILEOPENDIALOGOPTIONS.FOS_PICKFOLDERS | FILEOPENDIALOGOPTIONS.FOS_FORCEFILESYSTEM);

                // Show the dialog
                try
                {
                    fileDialog.Show(new HWND(hwnd));

                    // Get the result
                    fileDialog.GetResult(out var resultItem);

                    if (resultItem != null)
                    {
                        // Get the folder path
                        resultItem.GetDisplayName(SIGDN.SIGDN_FILESYSPATH, out var pathPtr);

                        string folderPath = new string(pathPtr.Value);

                        // Free the memory allocated by COM
                        Marshal.FreeCoTaskMem((IntPtr)pathPtr.Value);

                        StatusText.Text = "Folder selected successfully!";
                        ResultText.Text = $"Selected folder:\n{folderPath}";

                        // Release COM objects
                        Marshal.ReleaseComObject(resultItem);
                    }
                    else
                    {
                        StatusText.Text = "No folder was selected.";
                        ResultText.Text = string.Empty;
                    }
                }
                catch (COMException comEx) when (comEx.HResult == unchecked((int)0x800704C7))
                {
                    // User cancelled
                    StatusText.Text = "Folder picker was cancelled.";
                    ResultText.Text = string.Empty;
                }
                catch (COMException comEx)
                {
                    StatusText.Text = $"COM error occurred. HRESULT: 0x{comEx.HResult:X8}";
                    ResultText.Text = $"Message: {comEx.Message}";
                }

                Marshal.ReleaseComObject(fileDialog);
            }
            catch (Exception ex)
            {
                StatusText.Text = "Error occurred!";
                ResultText.Text = $"Exception: {ex.GetType().Name}\nMessage: {ex.Message}\n\nStack trace:\n{ex.StackTrace}";
            }
        }
        */

        private async void OpenFileButtonSDK_Click(object sender, RoutedEventArgs e)
        {
            var picker = new Microsoft.Windows.Storage.Pickers.FileOpenPicker(this.AppWindow.Id);
            var res = await picker.PickSingleFileAsync();
            if (res != null)
            {
                StatusText.Text = "SDK selected file successfully!";
                ResultText.Text = $"Selected file:\n{res.Path}";
            }
            else
            {
                StatusText.Text = "No file was selected.";
                ResultText.Text = string.Empty;
            }
        }

        private async void OpenFileButtonUWP_Click(object sender, RoutedEventArgs e)
        {
            var picker = new Windows.Storage.Pickers.FileOpenPicker();
            WinRT.Interop.InitializeWithWindow.Initialize(picker, WinRT.Interop.WindowNative.GetWindowHandle(this));
            picker.FileTypeFilter.Add("*");
            var res = await picker.PickSingleFileAsync();
            if (res != null)
            {
                StatusText.Text = "UWP selected file successfully!";
                ResultText.Text = $"Selected file:\n{res.Path}";
            }
            else
            {
                StatusText.Text = "No file was selected.";
                ResultText.Text = string.Empty;
            }
        }
    }
}
