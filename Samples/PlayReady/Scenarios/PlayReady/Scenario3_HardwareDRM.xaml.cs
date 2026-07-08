//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using SDKTemplate.ViewModels;
using Microsoft.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario3_HardwareDRM : Page
    {
        public Scenario3_HardwareDRM()
        {
            this.InitializeComponent();
            mediaElement.SetMediaPlayer(new Windows.Media.Playback.MediaPlayer());
            this.DataContext = new HardwareDRMViewModel(mediaElement);
        }
    }
}
