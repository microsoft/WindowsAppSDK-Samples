using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media;

namespace FocusVisualsSample;

public sealed class SocialMediaCounter : HyperlinkButton
{
    public static readonly DependencyProperty ShareImageProperty =
        DependencyProperty.Register(nameof(ShareImage), typeof(ImageSource), typeof(SocialMediaCounter), new PropertyMetadata(null));

    public ImageSource ShareImage
    {
        get => (ImageSource)GetValue(ShareImageProperty);
        set => SetValue(ShareImageProperty, value);
    }

    public static readonly DependencyProperty NumOfSharesProperty =
        DependencyProperty.Register(nameof(NumOfShares), typeof(int), typeof(SocialMediaCounter), new PropertyMetadata(0));

    public int NumOfShares
    {
        get => (int)GetValue(NumOfSharesProperty);
        set => SetValue(NumOfSharesProperty, value);
    }

    public SocialMediaCounter()
    {
        DefaultStyleKey = typeof(SocialMediaCounter);
    }
}
