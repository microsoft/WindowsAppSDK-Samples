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

namespace WpfWithIslandApp
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private void ShowAppWindowClick(object sender, RoutedEventArgs e)
        {
            var appWindow = Microsoft.UI.Windowing.AppWindow.Create();
            appWindow.Title = "I am an AppWindow";
            appWindow.Show();
        }

        private void CreateIslandClick(object sender, RoutedEventArgs e)
        {
            var wpfIslandHost = new WpfIslandHost();
            wpfIslandHost.Width = 400;
            wpfIslandHost.Height = 400;
            _rootPanel.Children.Add(wpfIslandHost);
        }
    }
}