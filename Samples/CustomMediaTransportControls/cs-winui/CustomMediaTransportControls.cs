// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

namespace CustomMediaTransportControls;

public sealed class CustomMediaTransportControls : MediaTransportControls
{
    private Button? likeButton;

    public CustomMediaTransportControls()
    {
        DefaultStyleKey = typeof(CustomMediaTransportControls);
    }

    public event EventHandler? Liked;

    protected override void OnApplyTemplate()
    {
        if (likeButton is not null)
        {
            likeButton.Click -= LikeButton_Click;
        }

        base.OnApplyTemplate();

        likeButton = GetTemplateChild("LikeButton") as Button;
        if (likeButton is not null)
        {
            likeButton.Click += LikeButton_Click;
        }
    }

    private void LikeButton_Click(object sender, RoutedEventArgs e)
    {
        Liked?.Invoke(this, EventArgs.Empty);
    }
}
