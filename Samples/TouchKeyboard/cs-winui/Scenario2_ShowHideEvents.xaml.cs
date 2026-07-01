using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Navigation;
using Windows.UI.ViewManagement;

namespace TouchKeyboard;

/// <summary>
/// Sample page to subscribe show/hide event of Touch Keyboard.
/// </summary>
public sealed partial class Scenario2_ShowHideEvents : Page
{
    private InputPane _inputPane;

    public Scenario2_ShowHideEvents()
    {
        InitializeComponent();
    }

    protected override void OnNavigatedTo(NavigationEventArgs e)
    {
        var window = MainWindow.Current;
        if (window is null)
        {
            return;
        }

        var hwnd = WinRT.Interop.WindowNative.GetWindowHandle(window);
        _inputPane = InputPaneInterop.GetForWindow(hwnd);

        _inputPane.Showing += OnShowing;
        _inputPane.Hiding += OnHiding;
    }

    protected override void OnNavigatedFrom(NavigationEventArgs e)
    {
        if (_inputPane is not null)
        {
            _inputPane.Showing -= OnShowing;
            _inputPane.Hiding -= OnHiding;
            _inputPane = null;
        }
    }

    private void OnShowing(InputPane sender, InputPaneVisibilityEventArgs e)
    {
        DispatcherQueue.TryEnqueue(() =>
        {
            LastInputPaneEventRun.Text = "Showing";
        });
    }

    private void OnHiding(InputPane sender, InputPaneVisibilityEventArgs e)
    {
        DispatcherQueue.TryEnqueue(() =>
        {
            LastInputPaneEventRun.Text = "Hiding";
        });
    }
}
