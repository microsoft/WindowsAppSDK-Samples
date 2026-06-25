// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using Microsoft.UI.Xaml.Controls;
using WindowsAISample;
using WindowsAISample.Models;
using WindowsAISample.ViewModels;

namespace WindowsAISample.Pages;

[NavPage("Image Foreground Extractor", Icon = Symbol.Filter, Order = 70)]
public sealed partial class ImageForegroundExtractorPage : Page
{
    public ImageForegroundExtractorPage()
    {
        InitializeComponent();
        DataContext = new ImageForegroundExtractorViewModel(new ImageForegroundExtractorModel());
    }
}
