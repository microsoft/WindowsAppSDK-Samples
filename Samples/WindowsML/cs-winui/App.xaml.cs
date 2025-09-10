// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.

using Microsoft.UI.Xaml;
using System;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace WindowsMLSample
{
    public partial class App : Application
    {
        protected override void OnLaunched(LaunchActivatedEventArgs args)
        {
            Window = new MainWindow();
            Window.Activate();
            WindowHandle = WinRT.Interop.WindowNative.GetWindowHandle(Window);
        }
        public static MainWindow? Window { get; private set; }
        public static IntPtr WindowHandle { get; private set; }
    }
}
