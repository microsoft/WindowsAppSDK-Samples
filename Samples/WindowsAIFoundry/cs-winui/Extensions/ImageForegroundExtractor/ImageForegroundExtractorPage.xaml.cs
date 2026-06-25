// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml.Controls;
using WindowsAISample;
using WindowsAISample.Ext.ImageForegroundExtractor.Models;
using WindowsAISample.Ext.ImageForegroundExtractor.ViewModels;

namespace WindowsAISample.Ext.ImageForegroundExtractor.Pages;

[NavPage("Image Foreground Extractor", Icon = Symbol.Filter, Order = 70)]
public sealed partial class ImageForegroundExtractorPage : Page
{
    public ImageForegroundExtractorPage()
    {
        InitializeComponent();
        DataContext = new ImageForegroundExtractorViewModel(new ImageForegroundExtractorModel());
    }
}
