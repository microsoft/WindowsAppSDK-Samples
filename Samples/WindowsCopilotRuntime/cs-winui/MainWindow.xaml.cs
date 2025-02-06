// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using WindowsCopilotRuntimeSample.ViewModels;
using WindowsCopilotRuntimeSample.Pages;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

namespace WindowsCopilotRuntimeSample;

/// <summary>
/// An empty window that can be used on its own or navigated to within a Frame.
/// </summary>
public sealed partial class MainWindow : Window
{
    public MainWindow()
    {
        InitializeComponent();
        rootFrame.DataContext = new CopilotRootViewModel();
        rootFrame.Navigate(typeof(LanguageModelPage));
    }

    private void NavView_SelectionChanged(NavigationView sender, NavigationViewSelectionChangedEventArgs args)
    {
        if (args.SelectedItemContainer != null)
        {
            switch (args.SelectedItemContainer.Tag)
            {
                case "LanguageModel":
                    rootFrame.Navigate(typeof(LanguageModelPage));
                    break;
                case "ImageScaler":
                    rootFrame.Navigate(typeof(ImageScalerPage));
                    break;
                case "ImageObjectExtractor":
                    rootFrame.Navigate(typeof(ImageObjectExtractorPage));
                    break;
                case "ImageDescription":
                    rootFrame.Navigate(typeof (ImageDescriptionPage));
                    break;
                case "TextRecognizer":
                    rootFrame.Navigate(typeof(TextRecognizerPage));
                    break;
            }
        }
    }
}
