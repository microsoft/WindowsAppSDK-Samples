// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using WindowsAISample.Shared;
using WindowsAISample.ViewModels;

namespace WindowsAISample;

/// <summary>
/// Hosts the NavigationView shell. Every nav item is contributed by a Page class decorated with
/// <see cref="NavPageAttribute"/>; this window discovers them via reflection at startup. Pages may live in the
/// main exe or in any extension assembly listed in <c>Extensions.props</c>.
///
/// To prevent a stale Ext.dll left in <c>bin/</c> from a previous experimental build from leaking into a
/// subsequent stable build, the discovery scope for extension assemblies is filtered against the build-emitted
/// <c>GeneratedExtensionsManifest.ExpectedAssemblyNames</c> set. The main exe's own assembly is always included.
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
        var mainAsmName = typeof(MainWindow).Assembly.GetName().Name ?? string.Empty;

        var expectedExt = new HashSet<string>(
            GeneratedExtensionsManifest.ExpectedAssemblyNames,
            StringComparer.OrdinalIgnoreCase);

        var appDir = Path.GetDirectoryName(typeof(MainWindow).Assembly.Location);
        if (!string.IsNullOrEmpty(appDir) && expectedExt.Count > 0)
        {
            var alreadyLoaded = new HashSet<string>(
                AppDomain.CurrentDomain.GetAssemblies()
                    .Select(a => a.GetName().Name)
                    .Where(n => !string.IsNullOrEmpty(n))!,
                StringComparer.OrdinalIgnoreCase);

            foreach (var dllPath in Directory.GetFiles(appDir, "WindowsAISample.Ext.*.dll"))
            {
                var name = Path.GetFileNameWithoutExtension(dllPath);
                if (!expectedExt.Contains(name) || alreadyLoaded.Contains(name))
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

        var includedAsms = new HashSet<string>(expectedExt, StringComparer.OrdinalIgnoreCase)
        {
            mainAsmName,
        };

        var discovered = AppDomain.CurrentDomain.GetAssemblies()
            .Where(a => includedAsms.Contains(a.GetName().Name ?? string.Empty))
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
