// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml.Controls;

namespace WindowsAISample.Ext.VideoScaler;

public sealed partial class VideoScalerPage : Page
{
    public VideoScalerPage()
    {
        InitializeComponent();
        DataContext = new VideoScalerViewModel(new VideoScalerModel());
    }
}
