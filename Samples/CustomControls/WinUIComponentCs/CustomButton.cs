using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Documents;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace WinUIComponentCs
{
    public sealed class CustomButton : Button
    {
        public string Text { get; private set; }
        public bool OverrideEntered { get; set; }

        public CustomButton()
            : this("CustomButton")
        {
        }

        public CustomButton(string text)
        {
            Text = text;
            Content = text;
            OverrideEntered = true;
        }

        protected override void OnPointerEntered(global::Microsoft.UI.Xaml.Input.PointerRoutedEventArgs e)
        {
            if (!OverrideEntered)
            {
                base.OnPointerEntered(e);
                return;
            }

            Text = Content?.ToString();
            Content = "Entered";
        }

        protected override void OnPointerExited(global::Microsoft.UI.Xaml.Input.PointerRoutedEventArgs e)
        {
            if (!OverrideEntered)
            {
                base.OnPointerExited(e);
                return;
            }

            Content = Text;
        }

        protected override Size MeasureOverride(Size availableSize)
        {
            var size = new Size(160, 30);
            base.MeasureOverride(size);
            return size;
        }

        public string GetText()
        {
            return Text;
        }
    }
}
