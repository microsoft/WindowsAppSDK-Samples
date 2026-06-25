// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using WindowsAISample;
using WindowsAISample.ViewModels;

namespace WindowsAISample;

/// <summary>
/// Hosts the NavigationView shell. Every nav item is contributed by a Page class decorated with
/// <see cref="NavPageAttribute"/>; this window discovers them via reflection at startup. Pages may live in the
/// main exe or in any extension assembly (WindowsAISample.Ext.*.dll) found in the app directory.
/// </summary>
public sealed partial class MainWindow : Window
{
    public MainWindow()
    {
        InitializeComponent();

        rootFrame.DataContext = new CopilotRootViewModel();

        DiscoverAndAppendNavPageItems();

        // Select the first nav item so initial Frame navigation happens via SelectionChanged like every other nav action.
        if (NavView.MenuItems.Count > 0)
        {
            NavView.SelectedItem = NavView.MenuItems[0];
        }
    }

    private void DiscoverAndAppendNavPageItems()
    {
        var appDir = Path.GetDirectoryName(typeof(MainWindow).Assembly.Location);
        if (!string.IsNullOrEmpty(appDir))
        {
            var alreadyLoaded = new HashSet<string>(
                AppDomain.CurrentDomain.GetAssemblies()
                    .Select(a => a.GetName().Name)
                    .Where(n => !string.IsNullOrEmpty(n))!,
                StringComparer.OrdinalIgnoreCase);

            foreach (var dllPath in Directory.GetFiles(appDir, "WindowsAISample.Ext.*.dll"))
            {
                var name = Path.GetFileNameWithoutExtension(dllPath);
                if (alreadyLoaded.Contains(name))
                {
                    continue;
                }
                try
                {
                    Assembly.LoadFrom(dllPath);
                }
                catch
                {
                    // Skip extensions we cannot load; the rest of the app continues to function.
                }
            }
        }

        var discovered = AppDomain.CurrentDomain.GetAssemblies()
            .SelectMany(SafeGetTypes)
            .Where(t => typeof(Page).IsAssignableFrom(t))
            .Select(t => (Type: t, Attr: t.GetCustomAttribute<NavPageAttribute>()))
            .Where(x => x.Attr is not null)
            .OrderBy(x => x.Attr!.Order)
            .ThenBy(x => x.Attr!.Title, StringComparer.Ordinal);

        foreach (var (type, attr) in discovered)
        {
            NavView.MenuItems.Add(new NavigationViewItem
            {
                Content = attr!.Title,
                Icon = new SymbolIcon(attr.Icon),
                Tag = type,
            });
        }
    }

    private static IEnumerable<Type> SafeGetTypes(Assembly assembly)
    {
        try
        {
            return assembly.GetTypes();
        }
        catch (ReflectionTypeLoadException ex)
        {
            return ex.Types.Where(t => t is not null)!;
        }
    }

    private void NavView_SelectionChanged(NavigationView sender, NavigationViewSelectionChangedEventArgs args)
    {
        if (args.SelectedItemContainer?.Tag is Type pageType)
        {
            rootFrame.Navigate(pageType);
        }
    }
}
