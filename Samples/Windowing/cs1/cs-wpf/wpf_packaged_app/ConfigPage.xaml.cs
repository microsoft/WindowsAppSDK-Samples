// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using System.Windows;
using System.Windows.Controls;
using Microsoft.UI.Windowing;
using WinRT;

namespace wpf_packaged_app
{
    public partial class ConfigPage : Page
    {
        private AppWindow _mainAppWindow = MainWindow.AppWindow;

        public ConfigPage()
        {
            InitializeComponent();
        }

        private void Page_Loaded(object sender, System.Windows.RoutedEventArgs e)
        {
            // Disable the switches that control properties only available when Overlapped if we're in any other Presenter state
            if (_mainAppWindow.Presenter.Kind != AppWindowPresenterKind.Overlapped)
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
            if (_mainAppWindow != null)
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

                _mainAppWindow.SetPresenter(customOverlappedPresenter);
            }
        }

        // Change the properties of the window based on which switch was toggled
        private void ChangeConfiguration(object sender, RoutedEventArgs e)
        {

            if (_mainAppWindow != null)
            {
                OverlappedPresenter overlappedPresenter = null;
                if (_mainAppWindow.Presenter.Kind == AppWindowPresenterKind.Overlapped)
                {
                    overlappedPresenter = _mainAppWindow.Presenter.As<OverlappedPresenter>();
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
                        if (_mainAppWindow.IsShownInSwitchers)
                        {
                            InUxToggle.Content = "Is not Shown in UX";
                        }
                        else
                        {
                            InUxToggle.Content = "Is Shown in UX";
                        }
                        _mainAppWindow.IsShownInSwitchers = !_mainAppWindow.IsShownInSwitchers;
                        break;

                    default:
                        // Something else called this method, we can't handle that so exit the method
                        return;
                }

            }
        }
    }
}
