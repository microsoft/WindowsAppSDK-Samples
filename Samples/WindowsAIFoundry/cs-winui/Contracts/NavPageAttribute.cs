// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using Microsoft.UI.Xaml.Controls;

namespace WindowsAISample;

/// <summary>
/// Marks a <see cref="Page"/> for inclusion in the main NavigationView.
///
/// The main window scans every loaded assembly at startup for Page types carrying this attribute and builds
/// the NavigationView dynamically. Extensions therefore self-register: there is no central list to edit when
/// a feature is added or promoted between experimental and stable.
/// </summary>
[AttributeUsage(AttributeTargets.Class, Inherited = false, AllowMultiple = false)]
public sealed class NavPageAttribute : Attribute
{
    public NavPageAttribute(string title)
    {
        Title = title;
    }

    public string Title { get; }

    public Symbol Icon { get; init; } = Symbol.Document;

    /// <summary>
    /// Lower values appear first. Use multiples of 10 to leave room for inserting new features without
    /// renumbering siblings.
    /// </summary>
    public int Order { get; init; } = 1000;
}
