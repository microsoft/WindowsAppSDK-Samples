// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

namespace WindowsAISample.Pages;

public sealed partial class LanguageModelPage : Page
{
    public LanguageModelPage()
    {
        InitializeComponent();
#if !WINAPPSDK_EXPERIMENTAL
        LoRASection.Visibility = Visibility.Collapsed;
#endif
    }
}
