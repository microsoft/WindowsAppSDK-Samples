// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using Microsoft.UI.Xaml.Controls;
using WindowsAISample.Shared;

namespace WindowsAISample.Pages;

/// <summary>
/// An empty page that can be used on its own or navigated to within a Frame.
/// </summary>
[NavPage("Image Object Remover", Icon = Symbol.Cut, Order = 70)]
public sealed partial class ImageObjectRemoverPage : Page
{
    public ImageObjectRemoverPage()
    {
        InitializeComponent();
    }
}
