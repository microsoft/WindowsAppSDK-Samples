// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml.Controls;
using WindowsAISample.Ext.ImageForegroundExtractor;

namespace WindowsAISample;

public sealed partial class MainWindow
{
    partial void AddImageForegroundExtractor()
    {
        InsertFeature(3, "Image Foreground Extractor", Symbol.ContactInfo, typeof(ImageForegroundExtractorPage));
    }
}
