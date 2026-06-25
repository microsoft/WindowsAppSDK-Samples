// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml.Controls;
using WindowsAISample;
using WindowsAISample.Ext.VideoScaler.Models;
using WindowsAISample.Ext.VideoScaler.ViewModels;

namespace WindowsAISample.Ext.VideoScaler.Pages;

[NavPage("Video Scaler", Icon = Symbol.Video, Order = 80)]
public sealed partial class VideoScalerPage : Page
{
    public VideoScalerPage()
    {
        InitializeComponent();
        DataContext = new VideoScalerViewModel(new VideoScalerModel());
    }
}