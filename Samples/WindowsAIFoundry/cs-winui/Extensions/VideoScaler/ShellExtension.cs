// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml.Controls;
using WindowsAISample.Ext.VideoScaler;

namespace WindowsAISample;

public sealed partial class MainWindow
{
    partial void AddVideoScaler()
    {
        AddFeature("Video Scaler", Symbol.Video, typeof(VideoScalerPage));
    }
}
