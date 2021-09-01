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
using WinRT;

namespace wpf_packaged_app
{
    /// <summary>
    /// Interaction logic for ConfigPage.xaml
    /// </summary>
    public partial class ConfigPage : Page
    {
        [DllImport("Microsoft.UI.Windowing.Core.dll", CharSet = CharSet.Unicode)]
        public static extern IntPtr GetWindowHandleFromWindowId(WindowId windowId, out IntPtr result);

        [DllImport("Microsoft.UI.Windowing.Core.dll", CharSet = CharSet.Unicode)]
        public static extern IntPtr GetWindowIdFromWindowHandle(IntPtr hwnd, out WindowId result);

        AppWindow m_mainAppWindow;
        AppWindowConfiguration windowConfiguration;

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
        public ConfigPage()
        {
            InitializeComponent();
        }
        private void Page_Loaded(object sender, System.Windows.RoutedEventArgs e)
        {
            m_mainAppWindow = GetAppWindowFromWPFWindow(Window.GetWindow(this));
            
        }
        private void ChangeWindowStyle(object sender, RoutedEventArgs e)
        {
            if (m_mainAppWindow != null)
            {
                AppWindowConfiguration windowConfiguration;

                switch (sender.As<Button>().Name)
                {
                    case "MainWindowBtn":
                        windowConfiguration = AppWindowConfiguration.CreateDefault();
                        break;

                    case "ContextMenuBtn":
                        windowConfiguration = AppWindowConfiguration.CreateForContextMenu();
                        break;

                    case "DialogWindowBtn":
                        windowConfiguration = AppWindowConfiguration.CreateForDialog();
                        break;

                    case "ToolWindowBtn":
                        windowConfiguration = AppWindowConfiguration.CreateForToolWindow();
                        break;

                    default:
                        // Something else called this method, we can't handle that so exit the method
                        return;
                }

                m_mainAppWindow.ApplyConfiguration(windowConfiguration);
            }
        }

        // Change the properties of the window based on which switch was toggled
        private void ChangeConfiguration(object sender, RoutedEventArgs e)
        {
            if (m_mainAppWindow != null)
            {
                AppWindowConfiguration windowConfiguration = m_mainAppWindow.Configuration;

                switch (sender.As<Button>().Name)
                {
                    case "FrameToggle":
                        if (windowConfiguration.HasFrame)
                        {
                            windowConfiguration.HasFrame = !windowConfiguration.HasFrame;
                            FrameToggle.Content = "Has No Frame";
                        }
                        else
                        {
                            windowConfiguration.HasFrame = !windowConfiguration.HasFrame;
                            FrameToggle.Content = "Has Frame";
                        }
                        break;

                    case "TitleBarToggle":         
                        if (windowConfiguration.HasTitleBar)
                        {
                            windowConfiguration.HasTitleBar = !windowConfiguration.HasTitleBar;
                            TitleBarToggle.Content = "Has No Title Bar";
                        }
                        else
                        {
                            windowConfiguration.HasTitleBar = !windowConfiguration.HasTitleBar;
                            TitleBarToggle.Content = "Has Title Bar";
                        }
                        break;

                    case "AlwaysOnTopToggle":
                        if (windowConfiguration.IsAlwaysOnTop)
                        {
                            windowConfiguration.IsAlwaysOnTop = !windowConfiguration.IsAlwaysOnTop;
                            AlwaysOnTopToggle.Content = "Is not Always on Top";
                        }
                        else
                        {
                            windowConfiguration.IsAlwaysOnTop = !windowConfiguration.IsAlwaysOnTop; ;
                            AlwaysOnTopToggle.Content = "Is Always on Top";
                        }
                        break;

                    case "MaxToggle":
                        if (windowConfiguration.IsMaximizable)
                        {
                            windowConfiguration.IsMaximizable = !windowConfiguration.IsMaximizable;
                            MaxToggle.Content = "Is not Maximizable";
                        }
                        else
                        {
                            windowConfiguration.IsMaximizable = !windowConfiguration.IsMaximizable;
                            MaxToggle.Content = "Is Maximizable";
                        }
                        break;

                    case "MinToggle":
                        if (windowConfiguration.IsMinimizable)
                        {
                            windowConfiguration.IsMinimizable = !windowConfiguration.IsMinimizable;
                            MinToggle.Content = "Is not Minimizable";
                        }
                        else
                        {
                            windowConfiguration.IsMinimizable = !windowConfiguration.IsMinimizable;
                            MinToggle.Content = "Is Minimizable";
                        }
                        break;

                    case "ResizableToggle":
                        if (windowConfiguration.IsResizable)
                        {
                            windowConfiguration.IsResizable = !windowConfiguration.IsResizable;
                            ResizableToggle.Content = "Is not Resizable";
                        }
                        else
                        {
                            windowConfiguration.IsResizable = !windowConfiguration.IsResizable;
                            ResizableToggle.Content = "Is Resizable";
                        }
                        break;

                    case "InUxToggle":         
                        if (windowConfiguration.IsShownInSwitchers)
                        {
                            windowConfiguration.IsShownInSwitchers = !windowConfiguration.IsShownInSwitchers;
                            InUxToggle.Content = "Is not Shown in UX";
                        }
                        else
                        {
                            windowConfiguration.IsShownInSwitchers = !windowConfiguration.IsShownInSwitchers;
                            InUxToggle.Content = "Is Shown in UX";
                        }
                        break;

                    default:
                        // Something else called this method, we can't handle that so exit the method
                        return;
                }

                m_mainAppWindow.ApplyConfiguration(windowConfiguration);
            }
        }
    }
}
