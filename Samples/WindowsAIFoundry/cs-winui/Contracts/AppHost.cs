// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using Microsoft.UI.Xaml;

namespace WindowsAISample;

/// <summary>
/// Host-app handles consumed by shared and extension code without taking a hard
/// dependency on the main app's <c>App</c> type.
/// </summary>
public static class AppHost
{
    public static Window? Window { get; set; }

    public static IntPtr WindowHandle { get; set; }
}
