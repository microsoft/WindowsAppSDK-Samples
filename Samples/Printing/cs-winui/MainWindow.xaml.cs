// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

namespace PrintingSample;

public sealed partial class MainWindow : Window
{
    public MainWindow()
    {
        InitializeComponent();
        AppWindow.SetIcon("Assets\\windows-sdk.ico");
    }

    public void Navigate(Type pageType, object parameter = null)
    {
        RootFrame.Navigate(pageType, parameter);
    }
}
