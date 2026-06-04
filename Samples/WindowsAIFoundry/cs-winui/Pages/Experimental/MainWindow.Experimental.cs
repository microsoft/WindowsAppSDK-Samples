// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml.Controls;
using WindowsAISample.Pages;

namespace WindowsAISample;

/// <summary>
/// Experimental-only behavior of <see cref="MainWindow"/>.
///
/// This file lives under <c>Pages/Experimental/</c> and is excluded from
/// compilation when <c>IncludeExperimentalApis=false</c>. When that happens,
/// the matching partial methods declared in <c>MainWindow.xaml.cs</c> are
/// elided by the compiler, the experimental NavView entries are removed at
/// startup, and the stable build is left with no experimental code paths.
///
/// To add a new experimental feature:
///   1. drop its Model / ViewModel / Page into the corresponding
///      <c>**/Experimental/</c> folder,
///   2. add a <c>&lt;NavigationViewItem Tag="Experimental:&lt;TagName&gt;" ...&gt;</c>
///      entry to <c>MainWindow.xaml</c>, and
///   3. add a matching <c>case</c> below.
///
/// To promote a feature to stable, move all of those files out of
/// <c>**/Experimental/</c>, drop the <c>Experimental:</c> prefix on its
/// NavItem tag, move the case into the stable switch in
/// <c>MainWindow.xaml.cs</c>, and remove its case here.
/// </summary>
public sealed partial class MainWindow
{
    partial void ApplyExperimentalToggle(ref bool enabled) => enabled = true;

    partial void NavigateExperimentalPage(string tag, Frame frame)
    {
        switch (tag)
        {
            case ExperimentalTagPrefix + "ImageForegroundExtractor":
                frame.Navigate(typeof(ImageForegroundExtractorPage));
                break;
            case ExperimentalTagPrefix + "VideoScaler":
                frame.Navigate(typeof(VideoScalerPage));
                break;
        }
    }
}
