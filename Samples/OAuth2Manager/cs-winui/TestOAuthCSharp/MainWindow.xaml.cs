// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.
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
using Microsoft.UI.Dispatching;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace TestOAuthCSharp
{
    /// <summary>
    /// An empty window that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainWindow : Window
    {
        private static DispatcherQueue dispatcherQueue;
        public MainWindow()
        {
            this.InitializeComponent();
            parentWindowId = GetWindowId();
            dispatcherQueue = DispatcherQueue.GetForCurrentThread();
        }

        private async void implicitGrantWithRedirectButton_Click(object sender, RoutedEventArgs e)
        {
            AuthRequestResult res = await OAuth2Manager.RequestAuthAsync(parentWindowId, new Uri("https://github.com/login/oauth/authorize?client_id=Ov23liJQ6xoz8ylsXkYs&scope=read%3Auser%20user%3Aemail"), new Uri("ms-testoauthcsharp-launch://oauthcallback"));

            //This is empty in case of github because github doesn't support implicit grant type.
            Debug.WriteLine($"AuthRequestResult Response Accesstoken: {res.Response.AccessToken}");
            outputTextBlock.Text = "Implicit OAuth with redirect performed : Auth res: " + res.ResponseUri;
        }

        private async void authCodeGrantWithRedirectButton_Click(object sender, RoutedEventArgs e)
        {
            var clientId = "your client id";
            var clientSecret = "your client secret";


            AuthRequestParams requestParams = AuthRequestParams.CreateForAuthorizationCodeRequest(clientId, new Uri("ms-testoauthcsharp-launch://oauthcallback"));
            requestParams.Scope = "read:user user:email";
            AuthRequestResult res = await OAuth2Manager.RequestAuthWithParamsAsync(parentWindowId, new Uri("https://github.com/login/oauth/authorize"), requestParams);

            Debug.WriteLine($"AuthRequestResult Response : {res.ResponseUri}");

            if (res.Response is not null)
            {
                TokenRequestParams tokenRequestParams = TokenRequestParams.CreateForAuthorizationCodeRequest(res.Response);

                // It is not ideal to expose the client secret in an app you distribute. 
                // Ideally this exchange should happen on a secure server.
                ClientAuthentication clientAuth = ClientAuthentication.CreateForBasicAuthorization(clientId, clientSecret);
                TokenRequestResult tokenRes = await OAuth2Manager.RequestTokenAsync(new Uri("https://github.com/login/oauth/access_token"), tokenRequestParams, clientAuth);
                var accessToken = tokenRes.Response.AccessToken;
                // Ensure this runs on the UI thread
                dispatcherQueue.TryEnqueue(()=>
                {
                    outputTextBlock.Text= "Auth code type TokenResponse : Token: " + accessToken + "\nAuth res: " + res.ResponseUri;
                });
            }
            else
            {
                dispatcherQueue.TryEnqueue(() =>
                {
                    outputTextBlock.Text = "Auth code type OAuth with redirect performed AuthResponse : " +  res.ResponseUri;
                });
            }
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
