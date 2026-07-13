//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Navigation;
using Windows.ApplicationModel.Activation;

namespace SDKTemplate
{
    /// <summary>
    /// Demonstrates receiving a protocol (URI) activation. When the app is launched or activated
    /// via an "alsdk:" URI, it is routed here with the activation payload (see Program.cs /
    /// App.RouteActivation).
    /// </summary>
    public sealed partial class Scenario4_ReceiveUri : Page
    {
        // A pointer back to the main page. This is needed if you want to call methods in MainPage such as NotifyUser().
        MainPage rootPage = MainPage.Current;

        string Protocol = "alsdk";

        public Scenario4_ReceiveUri()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame. The Parameter carries the
        /// protocol activation payload when the app was launched/activated by an "alsdk:" URI.
        /// </summary>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            var args = e.Parameter as IProtocolActivatedEventArgs;

            // Display the result of the protocol activation if we got here as a result of being
            // activated for a protocol.
            if (args != null)
            {
                rootPage.NotifyUser("Protocol activation received. The received URI is " + args.Uri.AbsoluteUri + ".", NotifyType.StatusMessage);
            }
        }
    }
}
