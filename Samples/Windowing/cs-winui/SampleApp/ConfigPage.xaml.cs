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
using WinRT;


namespace SampleApp
{
    public sealed partial class ConfigPage : Page
    {
        AppWindow m_mainAppWindow;

        public ConfigPage()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // Gets the AppWindow using the windowing interop methods (see WindowingInterop.cs for details)
            m_mainAppWindow = e.Parameter.As<Window>().GetAppWindow();
            base.OnNavigatedTo(e);
        }

        // Change the configuration of the window using a template
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

                switch (sender.As<ToggleSwitch>().Name)
                {
                    case "FrameToggle":
                        windowConfiguration.HasFrame = FrameToggle.IsOn;
                        break;

                    case "TitleBarToggle":
                        windowConfiguration.HasTitleBar = TitleBarToggle.IsOn;
                        break;

                    case "AlwaysOnTopToggle":
                        windowConfiguration.IsAlwaysOnTop = AlwaysOnTopToggle.IsOn;
                        break;

                    case "MaxToggle":
                        windowConfiguration.IsMaximizable = MaxToggle.IsOn;
                        break;

                    case "MinToggle":
                        windowConfiguration.IsMinimizable = MinToggle.IsOn;
                        break;

                    case "ResizableToggle":
                        windowConfiguration.IsResizable = ResizableToggle.IsOn;
                        break;

                    case "InUxToggle":
                        windowConfiguration.IsShownInSwitchers = InUxToggle.IsOn;
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
