// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using Microsoft.UI.Dispatching;
using Microsoft.UI.Xaml;

namespace PhotoEditor;

/// <summary>
/// Provides application-specific behavior to supplement the default Application class.
/// </summary>
public partial class App : Application
{
    /// <summary>
    /// Gets the application's main window. WinUI 3 desktop has no <c>Window.Current</c>,
    /// so the sample tracks the main window here (migration: Window.Current -> App.MainWindow).
    /// </summary>
    public static MainWindow MainWindow { get; private set; } = null!;

    /// <summary>
    /// Gets the DispatcherQueue for the UI thread. Replaces UWP's CoreDispatcher.
    /// </summary>
    public static DispatcherQueue MainDispatcherQueue { get; private set; } = null!;

    /// <summary>
    /// Gets the HWND of the main window, used to associate pickers with the window
    /// (migration: pickers must be initialized with the owning window handle).
    /// </summary>
    public static IntPtr WindowHandle { get; private set; }

    public App()
    {
        InitializeComponent();
    }

    protected override void OnLaunched(LaunchActivatedEventArgs args)
    {
        MainWindow = new MainWindow();
        MainDispatcherQueue = MainWindow.DispatcherQueue;
        WindowHandle = WinRT.Interop.WindowNative.GetWindowHandle(MainWindow);

        // Navigate only after MainWindow is assigned: pages read App.MainWindow in OnNavigatedTo.
        MainWindow.NavigateToMainPage();
        MainWindow.Activate();
    }
}
