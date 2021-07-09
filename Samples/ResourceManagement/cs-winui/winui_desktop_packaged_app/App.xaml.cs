// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

using System;
using System.Runtime.InteropServices;
using Microsoft.ApplicationModel.Resources;
using Microsoft.UI.Xaml;
using WinRT;


// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winui_desktop_packaged_app
{
    /// <summary>
    /// Provides application-specific behavior to supplement the default Application class.
    /// </summary>
    public partial class App : Application
    {
        /// <summary>
        /// Initializes the singleton application object.  This is the first line of authored code
        /// executed, and as such is the logical equivalent of main() or WinMain().
        /// </summary>
        public App()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when the application is launched normally by the end user.  Other entry points
        /// will be used such as when the application is launched to open a specific file.
        /// </summary>
        /// <param name="args">Details about the launch request and process.</param>
        protected override void OnLaunched(Microsoft.UI.Xaml.LaunchActivatedEventArgs args)
        {
            m_resourceLoader = new ResourceLoader();
            m_resourceManager = new ResourceManager();

            // Fall back to other resource loaders if the resource is not found in MRT, in this case .net.
            // This enables carrying forward existing assets without the need to convert them.
            m_resourceManager.ResourceNotFound += (sender, args) =>
            {
                var candidate = new ResourceCandidate(
                    ResourceCandidateKind.String,
                    LegacyResources.ResourceManager.GetString(args.Name, LegacyResources.Culture));

                args.SetResolvedCandidate(candidate);
            };

            m_window = new MainWindow(m_resourceLoader, m_resourceManager);

            // Get the Window's HWND
            var windowNative = m_window.As<IWindowNative>();
            // Needs to be set in code due to bug https://github.com/microsoft/microsoft-ui-xaml/issues/3689
            m_window.Title = "MRT Core C# sample";            
            m_window.Activate();

            // The Window object doesn't have Width and Height properties in WInUI 3 Desktop yet.
            // To set the Width and Height, you can use the Win32 API SetWindowPos.
            // Note, you should apply the DPI scale factor if you are thinking of DPI instead of pixels.
            SetWindowSize(windowNative.WindowHandle, 550, 500);
        }

        private void SetWindowSize(IntPtr hwnd, int width, int height)
        {
            var dpi = GetDpiForWindow(hwnd);
            float scalingFactor = (float)dpi / 96;
            width = (int)(width * scalingFactor);
            height = (int)(height * scalingFactor);

            SetWindowPos(hwnd, System.IntPtr.Zero, 0, 0, width, height, 2);
        }

        [DllImport("user32.dll", SetLastError = true)]
        static extern bool SetWindowPos(IntPtr hWnd, IntPtr hWndInsertAfter, int X, int Y, int cx, int cy, int uFlags);

        [DllImport("user32.dll", SetLastError = true)]
        static extern uint GetDpiForWindow(IntPtr hWnd);

        [ComImport]
        [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
        [Guid("EECDBF0E-BAE9-4CB6-A68E-9598E1CB57BB")]
        internal interface IWindowNative
        {
            IntPtr WindowHandle { get; }
        }

        private Window m_window;

        // These do disk I/O and should be centrally cached.
        // Since both end up loading the same file, ideally an app should only use one.
        // The resource manager is a superset of the resource loader in terms of functionality.
        private ResourceLoader m_resourceLoader;
        private ResourceManager m_resourceManager;
    }
}
