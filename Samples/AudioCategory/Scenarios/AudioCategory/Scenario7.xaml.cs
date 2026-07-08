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

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Windows.Media.Playback;

namespace SDKTemplate
{
    /// <summary>
    /// Plays an audio stream tagged with the Sound Effects audio category.
    /// </summary>
    public sealed partial class Scenario7 : Page
    {
        public Scenario7()
        {
            this.InitializeComponent();
        }

        private void Default_Click(object sender, RoutedEventArgs e)
        {
            Playback.SetAudioCategory(MediaPlayerAudioCategory.SoundEffects);
            Playback.SelectFile();
        }
    }
}
