using Microsoft.UI.Dispatching;
using SimpleWpfXamlIslandApp;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace SimpleWpfXamlIslandApp
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        DispatcherQueueController _controller;
        WinUIControlHost _winUIControl;

        public MainWindow()
        {
            _controller = Microsoft.UI.Dispatching.DispatcherQueueController.CreateOnCurrentThread();

            // var xamlApp = new XamlApp();
            //  var libXamlApp = new WinUILib.XamlApp();
            var winUIApp = new WinUIApp.App();
            InitializeComponent();
            Loaded += MainWindow_Loaded;
        }

        void MainWindow_Loaded(object sender, RoutedEventArgs e)
        {
            _winUIControl = new WinUIControlHost(ControlHostElement.ActualHeight, ControlHostElement.ActualWidth);
            ControlHostElement.Child = _winUIControl;
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            MessageBox.Show("Hello from WPF!");
        }
    }
}