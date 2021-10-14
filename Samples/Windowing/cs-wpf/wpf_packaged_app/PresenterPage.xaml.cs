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
    public partial class PresenterPage : Page
    {
        AppWindow m_mainAppWindow;

        public PresenterPage()
        {
            InitializeComponent();
        }

        private void Page_Loaded(object sender, System.Windows.RoutedEventArgs e)
        {
            m_mainAppWindow = AppWindowExtensions.GetAppWindowFromWPFWindow(Window.GetWindow(this));

            // Register for window changed events while on this page.
            m_mainAppWindow.Changed += AppWindowChangedHandler;
        }

        private void AppWindowChangedHandler(AppWindow sender, AppWindowChangedEventArgs args)
        {
            // The presenter changed so we need to update the button captions to reflect the new state
            if (args.DidPresenterChange)
            {
                switch (m_mainAppWindow.Presenter.Kind)
                {
                    case AppWindowPresenterKind.CompactOverlay:
                        CompactoverlaytBtn.Content = "Exit CompactOverlay";
                        FullscreenBtn.Content = "Enter FullScreen";
                        break;

                    case AppWindowPresenterKind.FullScreen:
                        CompactoverlaytBtn.Content = "Enter CompactOverlay";
                        FullscreenBtn.Content = "Exit FullScreen";
                        break;

                    case AppWindowPresenterKind.Overlapped:
                        CompactoverlaytBtn.Content = "Enter CompactOverlay";
                        FullscreenBtn.Content = "Enter FullScreen";
                        break;

                    default:
                        // If we end up here the presenter was changed to something we don't know what it is.
                        // This would happen if a new presenter is introduced.
                        // We can ignore this situation since we are not aware of the presenter and have no UI that
                        // reacts to this unknown presenter.
                        break;
                }
            }
        }

        private void SwitchPresenter(object sender, RoutedEventArgs e)
        {
            // Bail out if we don't have an AppWindow object.
            if (m_mainAppWindow != null)
            {

                AppWindowPresenterKind newPresenterKind;
                switch (sender.As<Button>().Name)
                {
                    case "CompactoverlaytBtn":
                        newPresenterKind = AppWindowPresenterKind.CompactOverlay;
                        break;

                    case "FullscreenBtn":
                        newPresenterKind = AppWindowPresenterKind.FullScreen;
                        break;

                    case "OverlappedBtn":
                        newPresenterKind = AppWindowPresenterKind.Overlapped;
                        break;

                    default:
                        newPresenterKind = AppWindowPresenterKind.Default;
                        break;
                }

                // If the same presenter button was pressed as the mode we're in, toggle the window back to Default.
                if (newPresenterKind == m_mainAppWindow.Presenter.Kind)
                {
                    m_mainAppWindow.SetPresenter(AppWindowPresenterKind.Default);
                }
                else
                {
                    // else request a presenter of the selected kind to be created and applied to the window.
                    m_mainAppWindow.SetPresenter(newPresenterKind);
                }
            }
        }
    }
}
