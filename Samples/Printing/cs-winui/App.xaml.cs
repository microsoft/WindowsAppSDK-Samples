// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;

namespace PrintingSample;

public partial class App : Application
{
    public static MainWindow MainWindow { get; private set; } = null!;

    public static nint MainWindowHandle => WinRT.Interop.WindowNative.GetWindowHandle(MainWindow);

    public App()
    {
        InitializeComponent();
    }

    protected override void OnLaunched(Microsoft.UI.Xaml.LaunchActivatedEventArgs args)
    {
        MainWindow = new MainWindow();
        MainWindow.Navigate(typeof(SDKTemplate.MainPage), args.Arguments);
        MainWindow.Activate();
    }
}
