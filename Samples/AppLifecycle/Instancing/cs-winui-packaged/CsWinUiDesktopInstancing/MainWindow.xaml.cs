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

namespace CsWinUiDesktopInstancing
{
    /// <summary>
    /// An empty window that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainWindow : Window
    {
        public MainWindow()
        {
            int numberOfActivations = ActivationTrackerHelper.GetNumberOfActivations();
            this.Title = numberOfActivations.ToString();
            this.InitializeComponent();
            activationTrackerTextBox.Text = numberOfActivations.ToString();
        }

        private void activationTrackerButton_Click(object sender, RoutedEventArgs e)
        {
            activationTrackerTextBox.Text = ActivationTrackerHelper.GetNumberOfActivations().ToString();
        }
    }
}
