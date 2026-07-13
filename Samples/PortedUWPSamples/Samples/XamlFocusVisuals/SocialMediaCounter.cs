using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Documents;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;

// The Templated Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234235

namespace SDKTemplate
{
    public sealed class SocialMediaCounter : HyperlinkButton
    {
        public static readonly DependencyProperty ShareImageProperty = DependencyProperty.Register("ShareImage", typeof(ImageSource), typeof(SocialMediaCounter), new PropertyMetadata(null));
        public ImageSource ShareImage
        {
            get { return (ImageSource)GetValue(ShareImageProperty); }
            set { SetValue(ShareImageProperty, value); }
        }

        public static readonly DependencyProperty NumOfSharesProperty = DependencyProperty.Register("NumOfShares", typeof(int), typeof(SocialMediaCounter), new PropertyMetadata(0));
        public int NumOfShares
        {
            get { return (int)GetValue(NumOfSharesProperty); }
            set { SetValue(NumOfSharesProperty, value); }
        }

        public SocialMediaCounter()
        {
            this.DefaultStyleKey = typeof(SocialMediaCounter);
        }
    }
}

