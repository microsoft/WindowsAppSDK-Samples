// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
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


        public ConfigPage()
        {
            InitializeComponent();
        }

        private void Page_Loaded(object sender, System.Windows.RoutedEventArgs e)
        {
            m_mainAppWindow = AppWindowExtensions.GetAppWindowFromWPFWindow(Window.GetWindow(this));

            // Disable the switches that control properties only available when Overlapped if we're in any other Presenter state
            if (m_mainAppWindow.Presenter.Kind != AppWindowPresenterKind.Overlapped)
            {
                FrameToggle.IsEnabled = false;
                TitleBarToggle.IsEnabled = false;
                AlwaysOnTopToggle.IsEnabled = false;
                MaxToggle.IsEnabled = false;
                MinToggle.IsEnabled = false;
                ResizableToggle.IsEnabled = false;
            }
            else
            {
                FrameToggle.IsEnabled = true;
                TitleBarToggle.IsEnabled = true;
                AlwaysOnTopToggle.IsEnabled = true;
                MaxToggle.IsEnabled = true;
                MinToggle.IsEnabled = true;
                ResizableToggle.IsEnabled = true;
            }
        }

        private void ChangeWindowStyle(object sender, RoutedEventArgs e)
        {
            if (m_mainAppWindow != null)
            {
                OverlappedPresenter customOverlappedPresenter;

                switch (sender.As<Button>().Name)
                {
                    case "MainWindowBtn":
                        customOverlappedPresenter = OverlappedPresenter.Create();
                        break;

                    case "ContextMenuBtn":
                        customOverlappedPresenter = OverlappedPresenter.CreateForContextMenu();
                        break;

                    case "DialogWindowBtn":
                        customOverlappedPresenter = OverlappedPresenter.CreateForDialog();
                        break;

                    case "ToolWindowBtn":
                        customOverlappedPresenter = OverlappedPresenter.CreateForToolWindow();
                        break;

                    default:
                        // Something else called this method, we can't handle that so exit the method
                        return;
                }

                m_mainAppWindow.SetPresenter(customOverlappedPresenter);
            }
        }

        // Change the properties of the window based on which switch was toggled
        private void ChangeConfiguration(object sender, RoutedEventArgs e)
        {

            if (m_mainAppWindow != null)
            {
                OverlappedPresenter overlappedPresenter = null;
                if (m_mainAppWindow.Presenter.Kind == AppWindowPresenterKind.Overlapped)
                {
                    overlappedPresenter = m_mainAppWindow.Presenter.As<OverlappedPresenter>();
                }

                switch (sender.As<Button>().Name)
                {
                    case "FrameToggle":
                        if (overlappedPresenter.HasBorder && overlappedPresenter.HasTitleBar)
                        {                          
                            FrameToggle.Content = "Has No Frame";
                        }
                        else
                        {
                            FrameToggle.Content = "Has Frame";
                        }
                        overlappedPresenter.SetBorderAndTitleBar(!overlappedPresenter.HasBorder, !overlappedPresenter.HasTitleBar);
                        break;

                    case "TitleBarToggle":         
                        if (overlappedPresenter.HasBorder && overlappedPresenter.HasTitleBar)
                        {
                            TitleBarToggle.Content = "Has No Title Bar";
                        }
                        else
                        {
                            TitleBarToggle.Content = "Has Title Bar";
                        }
                        overlappedPresenter.SetBorderAndTitleBar(!overlappedPresenter.HasBorder, !overlappedPresenter.HasTitleBar);
                        break;

                    case "AlwaysOnTopToggle":
                        if (overlappedPresenter.IsAlwaysOnTop)
                        {
                            AlwaysOnTopToggle.Content = "Is not Always on Top";
                        }
                        else
                        { 
                            AlwaysOnTopToggle.Content = "Is Always on Top";
                        }
                        overlappedPresenter.IsAlwaysOnTop = !overlappedPresenter.IsAlwaysOnTop;
                        break;

                    case "MaxToggle":
                        if (overlappedPresenter.IsMaximizable)
                        {
                            MaxToggle.Content = "Is not Maximizable";
                        }
                        else
                        {
                            MaxToggle.Content = "Is Maximizable";
                        }
                        overlappedPresenter.IsMaximizable = !overlappedPresenter.IsMaximizable;
                        break;

                    case "MinToggle":
                        if (overlappedPresenter.IsMinimizable)
                        {
                            MinToggle.Content = "Is not Minimizable";
                        }
                        else
                        {
                            MinToggle.Content = "Is Minimizable";
                        }
                        overlappedPresenter.IsMinimizable = !overlappedPresenter.IsMinimizable;
                        break;

                    case "ResizableToggle":
                        if (overlappedPresenter.IsResizable)
                        {
                            ResizableToggle.Content = "Is not Resizable";
                        }
                        else
                        {
                            ResizableToggle.Content = "Is Resizable";
                        }
                        overlappedPresenter.IsResizable = !overlappedPresenter.IsResizable;
                        break;

                    case "InUxToggle":         
                        if (m_mainAppWindow.IsShownInSwitchers)
                        {
                            InUxToggle.Content = "Is not Shown in UX";
                        }
                        else
                        {
                            InUxToggle.Content = "Is Shown in UX";
                        }
                        m_mainAppWindow.IsShownInSwitchers = !m_mainAppWindow.IsShownInSwitchers;
                        break;

                    default:
                        // Something else called this method, we can't handle that so exit the method
                        return;
                }

            }
        }
    }
}
