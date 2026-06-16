// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using Microsoft.UI.Xaml.Controls;
using WindowsAISample.Shared;

namespace WindowsAISample.Pages;

/// <summary>
/// An empty page that can be used on its own or navigated to within a Frame.
/// </summary>
[NavPage("Image Scaler", Icon = Symbol.FullScreen, Order = 20)]
public sealed partial class ImageScalerPage : Page
{
    public ImageScalerPage()
    {
        InitializeComponent();
    }
}
