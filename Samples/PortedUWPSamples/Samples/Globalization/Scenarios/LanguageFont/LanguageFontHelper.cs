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

using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media;
using Windows.Globalization.Fonts;

namespace SDKTemplate
{
    public static class LanguageFontHelper
    {
        public static void ApplyLanguageFont(TextBlock textBlock, LanguageFont languageFont)
        {
            FontFamily fontFamily = new FontFamily(languageFont.FontFamily);
            textBlock.FontFamily = fontFamily;
            textBlock.FontWeight = languageFont.FontWeight;
            textBlock.FontStyle = languageFont.FontStyle;
            textBlock.FontStretch = languageFont.FontStretch;
        }
    }
}