using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media;

namespace TouchKeyboard;

/// <summary>
/// Custom TextBox control derived from TextBox class.
/// </summary>
public class CustomTextBox : TextBox
{
    public CustomTextBox()
    {
        FontFamily = new FontFamily("Times New Roman");
        FontStyle = Windows.UI.Text.FontStyle.Italic;
        BorderThickness = new Thickness(1);
    }
}
