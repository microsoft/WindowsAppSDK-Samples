// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using System;
using System.Collections.Generic;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace WindowsCopilotRuntimeSample
{
    public partial class App : Application
    {
        public Dictionary<string, long> timerMap = [];
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