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
using System.Xml;

namespace WpfApp1
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

        private async void ButtonClick(object sender, RoutedEventArgs e)
        {
            try
            {
                var readyState = Microsoft.Windows.AI.Generative.LanguageModel.GetReadyState();
                System.Diagnostics.Debug.WriteLine($"LanguageModel.GetReadyState: {readyState}");
            }
            catch
            {
            }
        }
    }
}