// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System.Linq;
using WindowsAISample.ViewModels;
using WindowsAISample.Pages;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

namespace WindowsAISample;

/// <summary>
/// An empty window that can be used on its own or navigated to within a Frame.
///
/// Experimental-only behavior (which nav items survive, and which pages they
/// route to) is implemented in a sibling partial under
/// <c>Pages/Experimental/MainWindow.Experimental.cs</c>. That file is excluded
/// from compilation when <c>IncludeExperimentalApis=false</c>, so the partial
/// methods declared here become no-ops in the stable build and the
/// <c>Experimental:</c>-tagged nav items are hidden automatically.
/// </summary>
public sealed partial class MainWindow : Window
{
    private const string ExperimentalTagPrefix = "Experimental:";

    public MainWindow()
    {
        InitializeComponent();

        var experimentalEnabled = false;
        ApplyExperimentalToggle(ref experimentalEnabled);
        if (!experimentalEnabled)
        {
            HideExperimentalNavItems();
        }

        rootFrame.DataContext = new CopilotRootViewModel();
        rootFrame.Navigate(typeof(LanguageModelPage));
    }

    /// <summary>
    /// Implemented by the experimental partial; flips <paramref name="enabled"/>
    /// to <c>true</c> when the experimental build flavor is active. The stable
    /// build leaves this elided, so <paramref name="enabled"/> stays <c>false</c>.
    /// </summary>
    partial void ApplyExperimentalToggle(ref bool enabled);

    /// <summary>
    /// Implemented by the experimental partial to route to experimental pages.
    /// Elided in the stable build, where the corresponding nav items have
    /// already been removed and this code path is unreachable.
    /// </summary>
    partial void NavigateExperimentalPage(string tag, Frame frame);

    private void HideExperimentalNavItems()
    {
        var toRemove = NavView.MenuItems
            .OfType<NavigationViewItem>()
            .Where(item => item.Tag is string s && s.StartsWith(ExperimentalTagPrefix))
            .ToList();
        foreach (var item in toRemove)
        {
            NavView.MenuItems.Remove(item);
        }
    }

    private void NavView_SelectionChanged(NavigationView sender, NavigationViewSelectionChangedEventArgs args)
    {
        if (args.SelectedItemContainer?.Tag is not string tag)
        {
            return;
        }

        if (tag.StartsWith(ExperimentalTagPrefix))
        {
            NavigateExperimentalPage(tag, rootFrame);
            return;
        }

        switch (tag)
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