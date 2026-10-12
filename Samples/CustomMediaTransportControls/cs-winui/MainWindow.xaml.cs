// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Windows.Graphics;

namespace CustomMediaTransportControls;

public sealed partial class MainWindow : Window
{
    public MainWindow()
    {
        InitializeComponent();
        AppWindow.SetIcon("Assets/windows-sdk.ico");
        AppWindow.Resize(new SizeInt32(1000, 720));
    }
}
