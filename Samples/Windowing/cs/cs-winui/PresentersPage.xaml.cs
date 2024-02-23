// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Windowing;
using WinRT;


// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace Windowing
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Presenters : Page
    {
        private AppWindow _mainAppWindow = MainWindow.MyAppWindow;
        public Presenters()
        {
            this.InitializeComponent();
            // Register for window changed events while on this page.
            _mainAppWindow.Changed += AppWindowChangedHandler;
        }

        private void AppWindowChangedHandler(AppWindow sender, AppWindowChangedEventArgs args)
        {
            // The presenter changed so we need to update the button captions to reflect the new state
            if (args.DidPresenterChange)
            {
                switch (_mainAppWindow.Presenter.Kind)
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
            if (_mainAppWindow != null)
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
                if (newPresenterKind == _mainAppWindow.Presenter.Kind)
                {
                    _mainAppWindow.SetPresenter(AppWindowPresenterKind.Default);
                }
                else
                {
                    // else request a presenter of the selected kind to be created and applied to the window.
                    _mainAppWindow.SetPresenter(newPresenterKind);
                }
            }
        }
    }
}
