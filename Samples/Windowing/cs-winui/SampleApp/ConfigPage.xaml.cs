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

            base.OnNavigatedTo(e);
        }

        // Change the configuration of the window using a template
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

                switch (sender.As<ToggleSwitch>().Name)
                {
                    case "FrameToggle":
                        overlappedPresenter.SetBorderAndTitleBar(!overlappedPresenter.HasBorder, !overlappedPresenter.HasTitleBar);
                        break;

                    case "TitleBarToggle":
                        overlappedPresenter.SetBorderAndTitleBar(!overlappedPresenter.HasBorder, !overlappedPresenter.HasTitleBar);
                        break;

                    case "AlwaysOnTopToggle":
                        overlappedPresenter.IsAlwaysOnTop = AlwaysOnTopToggle.IsOn;
                        break;

                    case "MaxToggle":
                        overlappedPresenter.IsMaximizable = MaxToggle.IsOn;
                        break;

                    case "MinToggle":
                        overlappedPresenter.IsMinimizable = MinToggle.IsOn;
                        break;

                    case "ResizableToggle":
                        overlappedPresenter.IsResizable = ResizableToggle.IsOn;
                        break;

                    case "InUxToggle":
                        m_mainAppWindow.IsShownInSwitchers = InUxToggle.IsOn;
                        break;

                    default:
                        // Something else called this method, we can't handle that so exit the method
                        return;
                }
            }
        }
    }
}