// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

namespace SDKTemplate
{
    /// <summary>
    /// Custom TextBox control derived from TextBox class.
    /// </summary>
    public class CustomTextBox : TextBox
    {
        public CustomTextBox()
        {
            FontFamily = new Microsoft.UI.Xaml.Media.FontFamily("Times New Roman");
            FontStyle = Windows.UI.Text.FontStyle.Italic;
            BorderThickness = new Thickness(1);
        }
    }
}
