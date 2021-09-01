using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Interop;
using System.Runtime.InteropServices;
using Microsoft.UI;
using Microsoft.UI.Windowing;

//using Microsoft.UI.Xaml.Controls;

namespace wpf_packaged_app
{
    /// <summary>
    /// Interaction logic for DemoPage.xaml
    /// </summary>
    public partial class DemoPage : Page
    {
        [DllImport("Microsoft.UI.Windowing.Core.dll", CharSet = CharSet.Unicode)]
        public static extern IntPtr GetWindowHandleFromWindowId(WindowId windowId, out IntPtr result);

        [DllImport("Microsoft.UI.Windowing.Core.dll", CharSet = CharSet.Unicode)]
        public static extern IntPtr GetWindowIdFromWindowHandle(IntPtr hwnd, out WindowId result);

        AppWindow m_mainAppWindow;
        public static Microsoft.UI.Windowing.AppWindow GetAppWindowFromWPFWindow(Window wpfWindow)
        {
            // Get the HWND of the top level WPF window.
            var helper = new WindowInteropHelper(wpfWindow);
            IntPtr hwnd = (HwndSource.FromHwnd(helper.EnsureHandle())
                as IWin32Window).Handle;

            // Get the WindowId from the HWND.
            Microsoft.UI.WindowId windowId;
            GetWindowIdFromWindowHandle(hwnd, out windowId);

            // Get an AppWindow from the WindowId.
            Microsoft.UI.Windowing.AppWindow appWindow =
            Microsoft.UI.Windowing.AppWindow.GetFromWindowId(windowId);

            return appWindow;
        }
        public DemoPage()
        {
            InitializeComponent();
        }
        private void Page_Loaded(object sender, System.Windows.RoutedEventArgs e)
        {
            m_mainAppWindow = GetAppWindowFromWPFWindow(Window.GetWindow(this));
        }
        private void TitleBtn_Click(object sender, RoutedEventArgs e)
        {
            m_mainAppWindow.Title = TitleTextBox.Text;          
        }
        private void SizeBtn_Click(object sender, RoutedEventArgs e)
        {
            int windowWidth = 0;
            int windowHeight = 0;

            try
            {
                windowWidth = int.Parse(WidthTextBox.Text);
                windowHeight = int.Parse(HeightTextBox.Text);
            }
            catch (FormatException)
            {
                // Silently ignore invalid input...
            }

            if (windowHeight > 0 && windowWidth > 0)
            {
                m_mainAppWindow.Resize(new Windows.Graphics.SizeInt32(windowWidth, windowHeight));
            }
        }
    }
}
