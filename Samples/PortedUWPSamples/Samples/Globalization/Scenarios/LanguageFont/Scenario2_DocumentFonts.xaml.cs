//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using Microsoft.UI.Xaml.Navigation;
using System;
using Windows.Globalization.Fonts;
using Windows.Graphics.Display;
using Microsoft.UI.Xaml;
using Windows.UI.Text;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Media;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario2_DocumentFonts : Page
    {
        public Scenario2_DocumentFonts()
        {
            this.InitializeComponent();
        }

        private void ApplyFonts_Click(object sender, RoutedEventArgs e)
        {
            var languageFontGroup = new LanguageFontGroup("hi");

            LanguageFontHelper.ApplyLanguageFont(HeadingTextBlock, languageFontGroup.DocumentHeadingFont);

            // Not all scripts have recommended fonts for "document alternate"
            // categories, so need to verify before using it. Note that Hindi does
            // have document alternate fonts, so in this case the fallback logic is
            // unnecessary, but (for example) Japanese does not have recommendations
            // for the document alternate 2 category.
            LanguageFontHelper.ApplyLanguageFont(DocumentTextBlock,
                languageFontGroup.DocumentAlternate2Font ??
                languageFontGroup.DocumentAlternate1Font ??
                languageFontGroup.ModernDocumentFont);
        }
    }
}
