using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

namespace TouchKeyboard;

public sealed partial class MainWindow : Window
{
    public static new MainWindow Current { get; private set; }

    public MainWindow()
    {
        InitializeComponent();
        Current = this;

        ExtendsContentIntoTitleBar = true;
        SetTitleBar(AppTitleBar);
        AppWindow.SetIcon("Assets/AppIcon.ico");

        NavView.SelectedItem = NavView.MenuItems[0];
    }

    private void NavView_SelectionChanged(NavigationView sender, NavigationViewSelectionChangedEventArgs args)
    {
        if (args.SelectedItem is NavigationViewItem item)
        {
            var tag = item.Tag?.ToString();
            switch (tag)
            {
                case "Scenario1":
                    ContentFrame.Navigate(typeof(Scenario1_Launch));
                    break;
                case "Scenario2":
                    ContentFrame.Navigate(typeof(Scenario2_ShowHideEvents));
                    break;
                case "Scenario3":
                    ContentFrame.Navigate(typeof(Scenario3_ShowHideMethods));
                    break;
                case "Scenario4":
                    ContentFrame.Navigate(typeof(Scenario4_ShowView));
                    break;
            }
        }

        StatusInfoBar.IsOpen = false;
    }

    public void NotifyUser(string message, InfoBarSeverity severity)
    {
        DispatcherQueue.TryEnqueue(() =>
        {
            StatusInfoBar.Message = message;
            StatusInfoBar.Severity = severity;
            StatusInfoBar.IsOpen = !string.IsNullOrEmpty(message);
        });
    }
}
