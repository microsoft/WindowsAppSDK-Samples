// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;

namespace PhotoEditor;

/// <summary>
/// The application window. Hosts the photo collection and editor pages in a Frame,
/// with a Mica backdrop and a title bar that carries the app identity.
/// </summary>
public sealed partial class MainWindow : Window
{
    public MainWindow()
    {
        InitializeComponent();

        ExtendsContentIntoTitleBar = true;
        SetTitleBar(AppTitleBar);
    }

    /// <summary>
    /// Navigates the root frame to the collection page. Called from
    /// <see cref="App.OnLaunched"/> after <see cref="App.MainWindow"/> is assigned.
    /// </summary>
    public void NavigateToMainPage()
    {
        RootFrame.Navigate(typeof(MainPage));
    }
}
