//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************


using Windows.Globalization.Fonts;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;


namespace SDKTemplate
{
    public sealed partial class Scenario1_UIFonts : Page
    {
        public Scenario1_UIFonts()
        {
            this.InitializeComponent();
        }

        private void ApplyFonts_Click(object sender, RoutedEventArgs e)
        {
            var languageFontGroup = new LanguageFontGroup("ja-JP");

            // Change the Font value with selected font from LanguageFontGroup API
            LanguageFontHelper.ApplyLanguageFont(HeadingTextBlock, languageFontGroup.UIHeadingFont);
            LanguageFontHelper.ApplyLanguageFont(BodyTextBlock, languageFontGroup.UITextFont);
        }
    }
}
