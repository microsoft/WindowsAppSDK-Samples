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

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace WindowsCopilotRuntimeSample.Controls
{
    public sealed partial class PerformanceNotification : UserControl
    {

        public static readonly DependencyProperty ModelTypeProperty =
                    DependencyProperty.Register("ModelType", typeof(string), typeof(PerformanceNotification), new PropertyMetadata(default(string), OnChange));
        private string _content = string.Empty;

        public string ModelType
        {
            get { return (string)GetValue(ModelTypeProperty); }
            set { SetValue(ModelTypeProperty, value); }
        }

        private static void OnChange(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var control = (CodeBlockControl)d;
            //control.ExampleCode_Loaded(control, new RoutedEventArgs());
        }

        public PerformanceNotification()
        {
            this.InitializeComponent();
        }
    }
}
