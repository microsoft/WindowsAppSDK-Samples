// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using WindowsAISample.ViewModels;
using WindowsAISample.Pages;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using System.Linq;

namespace WindowsAISample;

/// <summary>
/// An empty window that can be used on its own or navigated to within a Frame.
/// </summary>
public sealed partial class MainWindow : Window
{
    public MainWindow()
    {
        InitializeComponent();
        rootFrame.DataContext = new CopilotRootViewModel();
        
#if !WINAPPSDK_EXPERIMENTAL
        // Remove experimental-only navigation items when not using experimental SDK
        var itemsToRemove = NavView.MenuItems
            .OfType<NavigationViewItem>()
            .Where(item => item.Tag is string tag && (tag == "VideoScaler" || tag == "ImageForegroundExtractor"))
            .ToList();
        foreach (var item in itemsToRemove)
        {
            NavView.MenuItems.Remove(item);
        }
#endif
        
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
#if WINAPPSDK_EXPERIMENTAL
                case "ImageForegroundExtractor":
                    rootFrame.Navigate(typeof(ImageForegroundExtractorPage));
                    break;
#endif
                case "ImageDescription":
                    rootFrame.Navigate(typeof (ImageDescriptionPage));
                    break;
                case "TextRecognizer":
                    rootFrame.Navigate(typeof(TextRecognizerPage));
                    break;
                case "ImageObjectRemover":
                    rootFrame.Navigate(typeof(ImageObjectRemoverPage));
                    break;
#if WINAPPSDK_EXPERIMENTAL
                case "VideoScaler":
                    rootFrame.Navigate(typeof(VideoScalerPage));
                    break;
#endif
            }
        }
    }
}