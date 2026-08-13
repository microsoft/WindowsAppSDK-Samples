// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using WindowsAISample.ViewModels;
using WindowsAISample.Pages;
using System;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

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
        AddImageForegroundExtractor();
        AddVideoScaler();
        rootFrame.Navigate(typeof(LanguageModelPage));
    }

    partial void AddImageForegroundExtractor();

    partial void AddVideoScaler();

    private void AddFeature(string title, Symbol icon, Type pageType)
    {
        NavView.MenuItems.Add(CreateFeature(title, icon, pageType));
    }

    private void InsertFeature(int index, string title, Symbol icon, Type pageType)
    {
        NavView.MenuItems.Insert(index, CreateFeature(title, icon, pageType));
    }

    private static NavigationViewItem CreateFeature(string title, Symbol icon, Type pageType)
    {
        return new NavigationViewItem
        {
            Content = title,
            Icon = new SymbolIcon(icon),
            Tag = pageType,
        };
    }

    private void NavView_SelectionChanged(NavigationView sender, NavigationViewSelectionChangedEventArgs args)
    {
        if (args.SelectedItemContainer != null)
        {
            switch (args.SelectedItemContainer.Tag)
            {
                case Type pageType:
                    rootFrame.Navigate(pageType);
                    break;
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
                    rootFrame.Navigate(typeof(ImageDescriptionPage));
                    break;
                case "TextRecognizer":
                    rootFrame.Navigate(typeof(TextRecognizerPage));
                    break;
                case "ImageObjectRemover":
                    rootFrame.Navigate(typeof(ImageObjectRemoverPage));
                    break;
            }
        }
    }
}