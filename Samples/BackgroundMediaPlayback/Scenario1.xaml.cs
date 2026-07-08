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

using SDKTemplate.DataModels;
using SDKTemplate.Services;
using SDKTemplate.ViewModels;

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Media;
using Windows.Media.Core;
using Windows.Media.Playback;
using Windows.Storage.Streams;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media.Imaging;
using Microsoft.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario1 : Page
    {
        MediaPlayer Player => PlaybackService.Instance.Player;

        MediaPlaybackList PlaybackList
        {
            get { return Player.Source as MediaPlaybackList; }
            set { Player.Source = value; }
        }
        MediaList MediaList
        {
            get { return PlaybackService.Instance.CurrentPlaylist; }
            set { PlaybackService.Instance.CurrentPlaylist = value; }
        }

        public PlayerViewModel PlayerViewModel { get; set; }

        public Scenario1()
        {
            this.InitializeComponent();

            // Never reuse the cached page because the model is designed to be unloaded and disposed
            this.NavigationCacheMode = NavigationCacheMode.Disabled;

            // Setup MediaPlayer view model
            PlayerViewModel = new PlayerViewModel(Player, DispatcherQueue);

            // Handle page load events
            Loaded += Scenario1_Loaded;
            Unloaded += Scenario1_Unloaded;

            // Respond to playback rate changes.
            Player.MediaPlayerRateChanged += Player_MediaPlayerRateChanged;
            UpdatePlaybackSpeed();
        }

        private async void Scenario1_Loaded(object sender, RoutedEventArgs e)
        {
            Debug.WriteLine("Scenario1_Loaded");
            
            // Update controls according to settings
            UpdateControlVisibility();
            SettingsService.Instance.UseCustomControlsChanged += SettingsService_UseCustomControlsChanged;

            // Bind player to element
            mediaPlayerElement.SetMediaPlayer(Player);

            // Load the playlist data model if needed
            if (MediaList == null)
            {
                // Create the playlist data model
                MediaList = new MediaList();
                await MediaList.LoadFromApplicationUriAsync("ms-appx:///Assets/playlist.json");
            }

            // Create a new playback list matching the data model if one does not exist
            if (PlaybackList == null)
                PlaybackList = MediaList.ToPlaybackList();

            // Subscribe to playback list item failure events
            PlaybackList.ItemFailed += PlaybackList_ItemFailed;

            // Create the view model list from the data model and playback model
            // and assign it to the player
            PlayerViewModel.MediaList = new MediaListViewModel(MediaList, PlaybackList, DispatcherQueue);
        }

        private void Scenario1_Unloaded(object sender, RoutedEventArgs e)
        {
            Debug.WriteLine("Scenario1_Unloaded");

            // Ensure the page is no longer holding references and force a GC
            // to ensure these are unloaded immediately since the app has
            // only a short timeframe to reduce working set to avoid suspending
            // on background transition.

            SettingsService.Instance.UseCustomControlsChanged -= SettingsService_UseCustomControlsChanged;

            PlaybackList.ItemFailed -= PlaybackList_ItemFailed;
            PlayerViewModel.Dispose();
            PlayerViewModel = null;

            GC.Collect();
        }

        private void SettingsService_UseCustomControlsChanged(object sender, EventArgs e)
        {
            UpdateControlVisibility();
        }

        void UpdateControlVisibility()
        {
            if (SettingsService.Instance.UseCustomControls)
            {
                mediaPlayerElement.AreTransportControlsEnabled = false;
                customButtons.Visibility = Visibility.Visible;
            }
            else
            {
                mediaPlayerElement.AreTransportControlsEnabled = true;
                customButtons.Visibility = Visibility.Collapsed;
            }
        }

        /// <summary>
        /// Handle item failures
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"></param>
        private void PlaybackList_ItemFailed(MediaPlaybackList sender, MediaPlaybackItemFailedEventArgs args)
        {
            // Media callbacks use a worker thread so dispatch to UI as needed
            DispatcherQueue.TryEnqueue(() =>
            {
                var error = string.Format("Item failed to play: {0} | 0x{1:x}",
                    args.Error.ErrorCode, args.Error.ExtendedError.HResult);
                MainPage.Current.NotifyUser(error, NotifyType.ErrorMessage);
            });
        }
        
        public void speedButton_Click(object sender, RoutedEventArgs e)
        {
            var menu = new MenuFlyout();
            foreach (var rate in new[] { 4.0, 2.0, 1.5, 1.0, 0.5 })
            {
                var item = new MenuFlyoutItem { Text = $"{rate:0.0}x" };
                item.Click += (s, args) => Player.PlaybackSession.PlaybackRate = rate;
                menu.Items.Add(item);
            }

            menu.ShowAt((FrameworkElement)sender);
        }

        private void Player_MediaPlayerRateChanged(MediaPlayer sender, MediaPlayerRateChangedEventArgs e)
        {
            // Media callbacks use a worker thread so dispatch to UI as needed
            DispatcherQueue.TryEnqueue(UpdatePlaybackSpeed);
        }

        private void UpdatePlaybackSpeed()
        {
            speedButton.Content = String.Format("{0:0.0}x", Player.PlaybackSession.PlaybackRate);
        }
    }
}
