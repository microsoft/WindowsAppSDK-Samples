using Microsoft.Security.Authentication.OAuth;
using Microsoft.UI;
using Microsoft.UI.Xaml;
using Microsoft.UI.Windowing;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Navigation;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using WinRT.Interop;
using System.Diagnostics;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace TestOAuthCSharp
{
    /// <summary>
    /// An empty window that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainWindow : Window
    {
        public MainWindow()
        {
            this.InitializeComponent();
            parentWindowId = GetWindowId();
        }

        private async void myButton_Click(object sender, RoutedEventArgs e)
        {
            AuthRequestResult res = await OAuth2Manager.RequestAuthAsync(parentWindowId, new Uri("https://github.com/login/oauth/authorize?client_id=Ov23liJQ6xoz8ylsXkYs&scope=read%3Auser%20user%3Aemail"), new Uri("ms-testoauthcsharp-launch://"));

            //This is empty in case of github because github doesn't support implicit grant type.
            Debug.WriteLine($"AuthRequestResult Response Accesstoken: {res.Response.AccessToken}");
            myButton.Content = "Clicked";
        }
        private WindowId GetWindowId()
        {
            // Get the window handle (HWND) for the current window
            var hwnd = WindowNative.GetWindowHandle(this);

            // Convert the HWND to a WindowId
            var windowId = Win32Interop.GetWindowIdFromWindow(hwnd);

            // Get the AppWindow for the current window
            var appWindow = AppWindow.GetFromWindowId(windowId);

            // Retrieve the WindowId
            var id = appWindow.Id;
            Debug.WriteLine($"WindowId: {id}");
            return id;
        }

        private WindowId parentWindowId;

        public void OnUriCallback(Uri responseUri)
        {
            if (!OAuth2Manager.CompleteAuthRequest(responseUri))
            {
                // The response is either invalid or does not correspond to any pending auth requests.
            }
            Debug.WriteLine($"responseUri: {responseUri}");
        }
    }
}
