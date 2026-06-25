// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml.Controls;
using WindowsAISample;
using WindowsAISample.Models;
using WindowsAISample.ViewModels;

namespace WindowsAISample.Pages;

[NavPage("Video Scaler", Icon = Symbol.Video, Order = 80)]
public sealed partial class VideoScalerPage : Page
{
    public VideoScalerPage()
    {
        InitializeComponent();
        DataContext = new VideoScalerViewModel(new VideoScalerModel());
    }
}