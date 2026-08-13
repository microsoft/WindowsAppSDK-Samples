// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml.Controls;

namespace WindowsAISample.Ext.ImageForegroundExtractor;

public sealed partial class ImageForegroundExtractorPage : Page
{
    public ImageForegroundExtractorPage()
    {
        InitializeComponent();
        DataContext = new ImageForegroundExtractorViewModel(new ImageForegroundExtractorModel());
    }
}
