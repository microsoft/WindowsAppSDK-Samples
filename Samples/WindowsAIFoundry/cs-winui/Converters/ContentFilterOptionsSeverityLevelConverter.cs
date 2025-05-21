// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Data;
using Microsoft.Windows.AI.ContentSafety;
using System;

namespace WindowsAISample.Converters;

internal partial class ContentFilterOptionsSeverityLevelConverter : IValueConverter
{
    public object Convert(object value, Type targetType, object parameter, string language)
    {
        if (value is SeverityLevel severityLevel)
        {
            return severityLevel == SeverityLevel.Medium ? "Medium (Default)" : severityLevel.ToString();
        }
        return string.Empty;
    }

    public object ConvertBack(object value, Type targetType, object parameter, string language)
    {
        if (value is string severityString)
        {
            if (severityString == "Medium (Default)")
            {
                return SeverityLevel.Medium;
            }
            if (Enum.TryParse(severityString, out SeverityLevel severityLevel))
            {
                return severityLevel;
            }
        }
        return SeverityLevel.Medium; // Default case
    }
}