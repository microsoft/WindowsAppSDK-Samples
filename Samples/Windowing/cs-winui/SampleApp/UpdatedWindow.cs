using Microsoft.UI;
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
    public class AppWindowPresenter2
    {
        protected AppWindowPresenterKind _kind = AppWindowPresenterKind.Overlapped;

        public AppWindowPresenterKind Kind => _kind;
    }

    public class OverlappedPresenter2 : AppWindowPresenter2
    {
        OverlappedPresenter2()
        {
            _kind = AppWindowPresenterKind.Overlapped;
        }

        private OverlappedPresenterState _State;

        public Boolean HasFrame;
        public Boolean HasTitleBar;
        public Boolean IsAlwaysOnTop;
        public Boolean IsMaximizable;
        public Boolean IsMinimizable;
        public Boolean IsModal;
        public Boolean IsResizable;

        public OverlappedPresenterState State => _State;

        public static OverlappedPresenter2 Create() {
            OverlappedPresenter2 presenter = new OverlappedPresenter2();
            presenter.HasFrame = true;
            presenter.HasTitleBar = true;
            presenter.IsAlwaysOnTop = false;
            presenter.IsMaximizable = true;
            presenter.IsMinimizable = true;
            presenter.IsModal = false;
            presenter.IsResizable = true;
            presenter._State = OverlappedPresenterState.Restored;
            return presenter;
        }

        public static OverlappedPresenter2 CreateForContextMenu() {
            OverlappedPresenter2 presenter = new OverlappedPresenter2();
            presenter.HasFrame = false;
            presenter.HasTitleBar = false;
            presenter.IsAlwaysOnTop = false;
            presenter.IsMaximizable = false;
            presenter.IsMinimizable = false;
            presenter.IsModal = false;
            presenter.IsResizable = false;
            presenter._State = OverlappedPresenterState.Restored;
            return presenter;
        }

        public static OverlappedPresenter2 CreateForDialog() {
            OverlappedPresenter2 presenter = new OverlappedPresenter2();
            presenter.HasFrame = true;
            presenter.HasTitleBar = true;
            presenter.IsAlwaysOnTop = false;
            presenter.IsMaximizable = false;
            presenter.IsMinimizable = false;
            presenter.IsModal = false;
            presenter.IsResizable = false;
            presenter._State = OverlappedPresenterState.Restored;
            return presenter;
        }

        public static OverlappedPresenter2 CreateForToolWindow() {
            OverlappedPresenter2 presenter = new OverlappedPresenter2();
            presenter.HasFrame = true;
            presenter.HasTitleBar = true;
            presenter.IsAlwaysOnTop = false;
            presenter.IsMaximizable = false;
            presenter.IsMinimizable = false;
            presenter.IsModal = false;
            presenter.IsResizable = true;
            presenter._State = OverlappedPresenterState.Restored;
            return presenter;
        }

        public void Maximize() { _State = OverlappedPresenterState.Maximized; }
        public void Minimize() { _State = OverlappedPresenterState.Minimized; }
        public void Restore() { _State = OverlappedPresenterState.Restored; }
    }

    public class CompactOverlayPresenter2 : AppWindowPresenter2
    {
        public CompactOverlaySize InitialSize;

        CompactOverlayPresenter2()
        {
            _kind = AppWindowPresenterKind.CompactOverlay;
            InitialSize = CompactOverlaySize.Small;
        }
    }

    public class FullScreenPresenter2 : AppWindowPresenter2
    {
        FullScreenPresenter2()
        {
            _kind = AppWindowPresenterKind.FullScreen;
        }
    }

    public class AppWindow2
    {
        private AppWindowPresenter2 _presenter;
        private WindowId _owner;
        private WindowId _id;
        private Boolean _isvisible;
        private Windows.Graphics.PointInt32 _position;
        private Windows.Graphics.SizeInt32 _size;
        private AppWindowTitleBar _TitleBar;

        public WindowId Id => _id;
        public Boolean IsShownInSwitchers;
        public Boolean IsVisible => _isvisible;
        public WindowId OwnerWindowId => _owner;
        public Windows.Graphics.PointInt32 Position => _position;
        public AppWindowPresenter2 Presenter => _presenter;
        public Windows.Graphics.SizeInt32 Size => _size;
        public String Title;
        public AppWindowTitleBar TitleBar => _TitleBar;

        public static AppWindow2 Create() {
            AppWindow2 appwindow = new AppWindow2();
            appwindow._presenter = OverlappedPresenter2.Create();
            return appwindow;
        }

        public static AppWindow2 Create(AppWindowPresenter2 appWindowPresenter) {
            AppWindow2 appwindow = new AppWindow2();
            appwindow._presenter = appWindowPresenter;
            return appwindow;
        }

        public static AppWindow2 Create(WindowId ownerWindowId) {
            AppWindow2 appwnd = new AppWindow2();
            appwnd._presenter = OverlappedPresenter2.Create();
            appwnd._owner = ownerWindowId;
            return appwnd;
        }

        public static AppWindow2 Create(AppWindowPresenter2 appWindowPresenter, WindowId ownerWindowId) {
            AppWindow2 appwnd = new AppWindow2();
            appwnd._presenter = appWindowPresenter;
            appwnd._owner = ownerWindowId;
            return appwnd;
        }

        public void Destroy() { }

        public void Hide() { }

        public void Move(Windows.Graphics.PointInt32 position) { }

        public void MoveAndResize(Windows.Graphics.RectInt32 rect) { }

        public void MoveAndResize(Windows.Graphics.RectInt32 rect, DisplayArea displayarea) { }

        public void Resize(Windows.Graphics.SizeInt32 size) { }

        public void SetIcon(String iconPath) { }

        public void SetIcon(Microsoft.UI.IconId iconId) { }

        public void Show() { }

        public void Show(Boolean activatewindow) { }

        public static AppWindow2 GetFromWindowId(WindowId windowId) { 
            AppWindow2 appwnd = new AppWindow2();
            appwnd._id = windowId;
            return appwnd;
        }

        public void SetPresenter(AppWindowPresenter2 presenter) {
            _presenter = presenter;
        }

        public void SetPresenter(AppWindowPresenterKind presenterKind) { }

        public event Windows.Foundation.TypedEventHandler<AppWindow2, AppWindowChangedEventArgs> Changed;
        public event Windows.Foundation.TypedEventHandler<AppWindow2, AppWindowClosingEventArgs> Closing;
        public event Windows.Foundation.TypedEventHandler<AppWindow2, Object> Destroying;
    }

    public sealed partial class ConfigPage2 : Page
    {
        AppWindow2 m_mainAppWindow;

        // Change the configuration of the window using a template
        private void ChangeWindowStyle(object sender, RoutedEventArgs e)
        {
            if (m_mainAppWindow != null)
            {
                OverlappedPresenter2 overlappedPresenter; // = OverlappedPresenter2.Create();

                switch (sender.As<Button>().Name)
                {
                    case "MainWindowBtn":
                        overlappedPresenter = OverlappedPresenter2.Create();
                        break;

                    case "ContextMenuBtn":
                        overlappedPresenter = OverlappedPresenter2.CreateForContextMenu();
                        break;

                    case "DialogWindowBtn":
                        overlappedPresenter = OverlappedPresenter2.CreateForDialog();
                        break;

                    case "ToolWindowBtn":
                        overlappedPresenter = OverlappedPresenter2.CreateForToolWindow();
                        break;

                    default:
                        // Something else called this method, we can't handle that so exit the method
                        return;
                }

                // Apply the new configuration to the window by setting the presenter.
                m_mainAppWindow.SetPresenter(overlappedPresenter);

                // Presenters does not automatically show the window, so call show if this window is not already visible.
                if (!m_mainAppWindow.IsVisible)
                {
                    m_mainAppWindow.Show();
                }
            }
        }

        // Change the properties of the window based on which switch was toggled
        private void ChangeConfiguration(object sender, RoutedEventArgs e)
        {
            AppWindow appWindow = AppWindow.Create();

            if (m_mainAppWindow != null)
            {
                // Some of the configurations are only available when an Overlapped Presenter is applied to the window
                if (m_mainAppWindow.Presenter.Kind == AppWindowPresenterKind.Overlapped)
                {
                    switch (sender.As<ToggleSwitch>().Name)
                    {
                        case "FrameToggle":
                            m_mainAppWindow.Presenter.As<OverlappedPresenter2>().HasFrame = FrameToggle.IsOn;
                            break;

                        case "TitleBarToggle":
                            m_mainAppWindow.Presenter.As<OverlappedPresenter2>().HasTitleBar = TitleBarToggle.IsOn;
                            break;

                        case "AlwaysOnTopToggle":
                            m_mainAppWindow.Presenter.As<OverlappedPresenter2>().IsAlwaysOnTop = AlwaysOnTopToggle.IsOn;
                            break;

                        case "MaxToggle":
                            m_mainAppWindow.Presenter.As<OverlappedPresenter2>().IsMaximizable = MaxToggle.IsOn;
                            break;

                        case "MinToggle":
                            m_mainAppWindow.Presenter.As<OverlappedPresenter2>().IsMinimizable = MinToggle.IsOn;
                            break;

                        case "ResizableToggle":
                            m_mainAppWindow.Presenter.As<OverlappedPresenter2>().IsResizable = ResizableToggle.IsOn;
                            break;

                        default:
                            // Something else called this method, we can't handle that so exit the method
                            return;
                    }
                }
                else if (sender.As<ToggleSwitch>().Name == "InUxToggle")
                {
                    m_mainAppWindow.IsShownInSwitchers = InUxToggle.IsOn;
                }
            }
        }
    }
}
