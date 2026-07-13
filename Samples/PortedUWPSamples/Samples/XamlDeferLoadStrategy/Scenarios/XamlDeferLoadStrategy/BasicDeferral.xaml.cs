using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

namespace SDKTemplate
{
    /// <summary>
    /// A page that demonstrates basic x:DeferLoadStrategy usage.
    /// </summary>
    public sealed partial class BasicDeferral : Page
    {
        public BasicDeferral()
        {
            this.InitializeComponent();
        }

        private void RealizeElements_Click(object sender, RoutedEventArgs e)
        {
            this.FindName("DeferredGrid"); //This will realize the deferred grid
        }
    }
}
