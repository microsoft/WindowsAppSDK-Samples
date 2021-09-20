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
    public partial class ConfigPage : Page
    {
        AppWindow m_mainAppWindow;
        AppWindowConfiguration windowConfiguration;

        public ConfigPage()
        {
            InitializeComponent();
        }

        private void Page_Loaded(object sender, System.Windows.RoutedEventArgs e)
        {
            m_mainAppWindow = AppWindowExtensions.GetAppWindowFromWPFWindow(Window.GetWindow(this));
            
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
                            FrameToggle.Content = "Has No Frame";
                        }
                        else
                        {
                            FrameToggle.Content = "Has Frame";
                        }
                        windowConfiguration.HasFrame = !windowConfiguration.HasFrame;
                        break;

                    case "TitleBarToggle":         
                        if (windowConfiguration.HasTitleBar)
                        {
                            TitleBarToggle.Content = "Has No Title Bar";
                        }
                        else
                        {
                            TitleBarToggle.Content = "Has Title Bar";
                        }
                        windowConfiguration.HasTitleBar = !windowConfiguration.HasTitleBar;
                        break;

                    case "AlwaysOnTopToggle":
                        if (windowConfiguration.IsAlwaysOnTop)
                        {
                            AlwaysOnTopToggle.Content = "Is not Always on Top";
                        }
                        else
                        { 
                            AlwaysOnTopToggle.Content = "Is Always on Top";
                        }
                        windowConfiguration.IsAlwaysOnTop = !windowConfiguration.IsAlwaysOnTop;
                        break;

                    case "MaxToggle":
                        if (windowConfiguration.IsMaximizable)
                        {
                            MaxToggle.Content = "Is not Maximizable";
                        }
                        else
                        {
                            MaxToggle.Content = "Is Maximizable";
                        }
                        windowConfiguration.IsMaximizable = !windowConfiguration.IsMaximizable;
                        break;

                    case "MinToggle":
                        if (windowConfiguration.IsMinimizable)
                        {
                            MinToggle.Content = "Is not Minimizable";
                        }
                        else
                        {
                            MinToggle.Content = "Is Minimizable";
                        }
                        windowConfiguration.IsMinimizable = !windowConfiguration.IsMinimizable;
                        break;

                    case "ResizableToggle":
                        if (windowConfiguration.IsResizable)
                        {
                            ResizableToggle.Content = "Is not Resizable";
                        }
                        else
                        {
                            ResizableToggle.Content = "Is Resizable";
                        }
                        windowConfiguration.IsResizable = !windowConfiguration.IsResizable;
                        break;

                    case "InUxToggle":         
                        if (windowConfiguration.IsShownInSwitchers)
                        {
                            InUxToggle.Content = "Is not Shown in UX";
                        }
                        else
                        {
                            InUxToggle.Content = "Is Shown in UX";
                        }
                        windowConfiguration.IsShownInSwitchers = !windowConfiguration.IsShownInSwitchers;
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
