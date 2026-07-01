using System;
using System.Runtime.InteropServices;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Windows.UI.ViewManagement.Core;
using WinRT.Interop;

namespace TouchKeyboard;

/// <summary>
/// Sample page to show the Emoji keyboard.
/// </summary>
public sealed partial class Scenario4_ShowView : Page
{
    public Scenario4_ShowView()
    {
        InitializeComponent();
    }

    private void TextBox_GotFocus(object sender, RoutedEventArgs e)
    {
        try
        {
            var window = MainWindow.Current;
            if (window is null) return;

            var hwnd = WindowNative.GetWindowHandle(window);

            var interopIid = new Guid("0576AB31-A310-4C40-B31F-CECDAE573C13");
            var coreInputViewIid = typeof(CoreInputView).GUID;

            // Create HString manually
            int hr = WindowsCreateString("Windows.UI.ViewManagement.Core.CoreInputView", 51, out var hstring);
            if (hr != 0)
            {
                MainWindow.Current?.NotifyUser("Failed to create activation string.", InfoBarSeverity.Warning);
                return;
            }

            try
            {
                hr = RoGetActivationFactory(hstring, ref interopIid, out var factoryPtr);
                if (hr != 0 || factoryPtr == IntPtr.Zero)
                {
                    MainWindow.Current?.NotifyUser("CoreInputView activation factory not available.", InfoBarSeverity.Warning);
                    return;
                }

                try
                {
                    var interop = (ICoreInputViewInterop)Marshal.GetObjectForIUnknown(factoryPtr);
                    interop.GetForWindow(hwnd, ref coreInputViewIid, out var resultPtr);

                    if (resultPtr != IntPtr.Zero)
                    {
                        try
                        {
                            var coreInputView = (CoreInputView)Marshal.GetObjectForIUnknown(resultPtr);
                            coreInputView.TryShow(CoreInputViewKind.Emoji);
                        }
                        finally
                        {
                            Marshal.Release(resultPtr);
                        }
                    }
                }
                finally
                {
                    Marshal.Release(factoryPtr);
                }
            }
            finally
            {
                WindowsDeleteString(hstring);
            }
        }
        catch (Exception ex)
        {
            MainWindow.Current?.NotifyUser($"Error showing emoji keyboard: {ex.Message}", InfoBarSeverity.Error);
        }
    }

    [DllImport("combase.dll", PreserveSig = true)]
    private static extern int RoGetActivationFactory(
        IntPtr activatableClassId,
        ref Guid iid,
        out IntPtr factory);

    [DllImport("combase.dll", PreserveSig = true)]
    private static extern int WindowsCreateString(
        [MarshalAs(UnmanagedType.LPWStr)] string sourceString,
        int length,
        out IntPtr hstring);

    [DllImport("combase.dll", PreserveSig = true)]
    private static extern int WindowsDeleteString(IntPtr hstring);

    [ComImport]
    [Guid("0576AB31-A310-4C40-B31F-CECDAE573C13")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    private interface ICoreInputViewInterop
    {
        void GetForWindow(nint appWindow, ref Guid riid, out IntPtr coreInputView);
    }
}
