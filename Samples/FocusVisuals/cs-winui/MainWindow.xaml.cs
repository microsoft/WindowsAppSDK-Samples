// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using FocusVisualsSample.Scenarios;

namespace FocusVisualsSample;

public sealed partial class MainWindow : Window
{
    public MainWindow()
    {
        InitializeComponent();

        ExtendsContentIntoTitleBar = true;
        SetTitleBar(AppTitleBar);

        AppWindow.SetIcon("Assets/AppIcon.ico");

        // Select the first navigation item on startup.
        NavView.SelectedItem = NavView.MenuItems[0];
    }

    private void NavView_SelectionChanged(NavigationView sender, NavigationViewSelectionChangedEventArgs args)
    {
        if (args.SelectedItem is NavigationViewItem item)
        {
            var tag = item.Tag?.ToString();
            switch (tag)
            {
                case "CustomFocusVisuals":
                    ContentFrame.Navigate(typeof(CustomFocusVisualsPage));
                    break;
                case "CustomControlUsage":
                    ContentFrame.Navigate(typeof(CustomControlUsagePage));
                    break;
            }
        }
    }
}
