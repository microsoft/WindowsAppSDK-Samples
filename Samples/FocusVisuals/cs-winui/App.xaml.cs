using Microsoft.UI.Xaml;

namespace FocusVisualsSample;

public partial class App : Application
{
    public App()
    {
        InitializeComponent();
    }

    protected override void OnLaunched(Microsoft.UI.Xaml.LaunchActivatedEventArgs args)
    {
        var window = new MainWindow();
        window.Activate();
    }
}
