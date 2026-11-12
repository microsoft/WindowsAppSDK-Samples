// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;

namespace WindowsAISample;

/// <summary>
/// Host-app state consumed by shared and extension code without depending on the main app's <c>App</c> type.
/// </summary>
public static class AppHost
{
    public static IntPtr WindowHandle { get; set; }
}
