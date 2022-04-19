using Microsoft.UI;
using Microsoft.UI.Windowing;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Navigation;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using WinRT.Interop;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace CustomEditControlSampleCs
{
    /// <summary>
    /// An empty window that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainWindow : Window
    {
        public MainWindow()
        {
            this.InitializeComponent();
            Content.PointerPressed += OnPointerPressed;

            IntPtr hWnd = WindowNative.GetWindowHandle((Window)this);
            WindowId windowId = Win32Interop.GetWindowIdFromWindow(hWnd);
            MyCustomEditControl.SetAppWindow(windowId);
        }

        public static Rect GetElementRect(FrameworkElement element)
        {
            GeneralTransform transform = element.TransformToVisual(null);
            Point point = transform.TransformPoint(new Point());
            return new Rect(point, new Size(element.ActualWidth, element.ActualHeight));
        }

        private void OnPointerPressed(object sender, PointerRoutedEventArgs e)
        {
            Rect contentRect = GetElementRect(MyCustomEditControl);
            if (contentRect.Contains(e.GetCurrentPoint(null).Position))
            {
                // The user tapped inside the control. Give it focus.
                MyCustomEditControl.SetInternalFocus();

                // Tell XAML that this element has focus, so we don't have two
                // focus elements. That is the extent of our integration with XAML focus.
                var ret = MyCustomEditControl.Focus(FocusState.Programmatic);
                if (ret)
                {
                    int i = 0; i++;
                }
                else
                {
                    int j = 0; j++;
                }

                // A more complete custom control would move the caret to the
                // pointer position. It would also provide some way to select
                // text via touch. We do neither in this sample.

            }
            else
            {
                // The user tapped outside the control. Remove focus.
                MyCustomEditControl.RemoveInternalFocus();
            }
        }
    }
}
