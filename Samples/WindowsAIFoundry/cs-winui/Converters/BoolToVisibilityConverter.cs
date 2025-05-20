// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Data;
using System;

namespace WindowsAISampleSample.Converters;

internal partial class BoolToVisibilityConverter : IValueConverter
{
    public object Convert(object value, Type targetType, object parameter, string language)
    {
        var result = System.Convert.ToBoolean(value);
        if (System.Convert.ToBoolean(parameter))
        {
            result = !result;
        }

        return result ? Visibility.Visible : Visibility.Collapsed;
    }

    public object ConvertBack(object value, Type targetType, object parameter, string language)
    {
        throw new NotImplementedException();
    }
}
