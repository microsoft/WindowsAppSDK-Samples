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

using Microsoft.UI.Xaml;

namespace SDKTemplate
{
    /// <summary>
    /// The application window. Hosts the SDKTemplate shell page (MainPage) in a Frame.
    /// </summary>
    public sealed partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

            ExtendsContentIntoTitleBar = true;
            SetTitleBar(AppTitleBar);
        }

        /// <summary>
        /// Navigates to the SDKTemplate shell page. Called from App after
        /// <see cref="App.MainWindow"/> has been assigned, so scenario pages can safely
        /// reference App.MainWindow.
        /// </summary>
        public void NavigateToMainPage()
        {
            RootFrame.Navigate(typeof(MainPage));
        }
    }
}
