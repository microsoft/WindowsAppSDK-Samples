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
using System.Collections.Generic;
using System.Diagnostics;
using Windows.Media.Playback;

namespace SDKTemplate
{
    /// <summary>
    /// Single-page Audio Category sample. The user picks an audio category from a dropdown, chooses a
    /// file, and plays it. Categories are most meaningful when two apps play at once, so the page also
    /// launches a second instance of itself and explains what interaction to expect.
    /// </summary>
    public sealed partial class Scenario1 : Page
    {
        private sealed class AudioCategoryOption
        {
            public string Name { get; init; }
            public MediaPlayerAudioCategory Category { get; init; }
            public string Description { get; init; }
        }

        private static readonly List<AudioCategoryOption> Options = new()
        {
            new() { Name = "Media", Category = MediaPlayerAudioCategory.Media,
                Description = "General audio/music playback. Attenuated by Communications and Alerts streams; also attenuates Game Media streams." },
            new() { Name = "Movie", Category = MediaPlayerAudioCategory.Movie,
                Description = "Video playback with a focus on clear dialog (movies). Behaves like Media but with movie-tuned effects." },
            new() { Name = "Game Chat", Category = MediaPlayerAudioCategory.GameChat,
                Description = "In-game voice chat. Behaves like Communications, except it does not attenuate other audio streams." },
            new() { Name = "Speech", Category = MediaPlayerAudioCategory.Speech,
                Description = "Speech input and output, such as personal assistants and navigation apps." },
            new() { Name = "Communications", Category = MediaPlayerAudioCategory.Communications,
                Description = "VOIP and calls. Attenuates Media and Movie streams while active, and can route to a communications endpoint (e.g. a Bluetooth HFP headset)." },
            new() { Name = "Alerts", Category = MediaPlayerAudioCategory.Alerts,
                Description = "Notification and alert sounds. Briefly attenuates Media and Movie streams while the alert plays." },
            new() { Name = "Sound Effects", Category = MediaPlayerAudioCategory.SoundEffects,
                Description = "Application sound effects." },
            new() { Name = "Game Effects", Category = MediaPlayerAudioCategory.GameEffects,
                Description = "Game sound effects." },
            new() { Name = "Game Media", Category = MediaPlayerAudioCategory.GameMedia,
                Description = "In-game background music. Similar to Media, but a Media stream will attenuate a Game Media stream." },
            new() { Name = "Other", Category = MediaPlayerAudioCategory.Other,
                Description = "Generic sounds. Lowest priority; plays only in the foreground and mixes with background media." },
        };

        public Scenario1()
        {
            this.InitializeComponent();
            CategoryComboBox.ItemsSource = Options;
            CategoryComboBox.SelectedIndex = 0;
        }

        private void CategoryComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (CategoryComboBox.SelectedItem is AudioCategoryOption option)
            {
                Playback.SetAudioCategory(option.Category);
                CategoryDescription.Text = option.Description;
            }
        }

        private void SelectFile_Click(object sender, RoutedEventArgs e)
        {
            Playback.SelectFile();
        }

        private void LaunchAnotherInstance_Click(object sender, RoutedEventArgs e)
        {
            // Launch a second instance of this packaged app via shell activation so it runs with the
            // app's package identity. WinUI 3 packaged apps are multi-instance by default.
            string aumid = Windows.ApplicationModel.AppInfo.Current.AppUserModelId;
            Process.Start(new ProcessStartInfo
            {
                FileName = "explorer.exe",
                Arguments = $"shell:AppsFolder\\{aumid}",
                UseShellExecute = true
            });
        }
    }
}
