// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml.Controls;
using Windows.Media.Core;

namespace CustomMediaTransportControls;

public sealed partial class MainPage : Page
{
    private int likeCount;

    public MainPage()
    {
        InitializeComponent();
        string mediaPath = Path.Combine(AppContext.BaseDirectory, "Assets", "fishes.wmv");
        MediaPlayer.Source = MediaSource.CreateFromUri(new Uri(mediaPath));
    }

    private async void CustomMediaTransportControls_Liked(object sender, EventArgs e)
    {
        likeCount++;

        ContentDialog likedDialog = new()
        {
            Title = "Video liked",
            Content = $"You liked this video {likeCount} time{(likeCount == 1 ? string.Empty : "s")}.",
            CloseButtonText = "Close",
            XamlRoot = XamlRoot
        };

        await likedDialog.ShowAsync();
    }
}
