// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using Microsoft.UI.Xaml.Controls;
using WindowsAISample;

namespace WindowsAISample.Pages;

/// <summary>
/// An empty page that can be used on its own or navigated to within a Frame.
/// </summary>
[NavPage("Text Recognizer", Icon = Symbol.Directions, Order = 60)]
public sealed partial class TextRecognizerPage : Page
{
    public TextRecognizerPage()
    {
        InitializeComponent();
    }
}
