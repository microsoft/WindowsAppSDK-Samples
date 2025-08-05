using Microsoft.UI;
using System.Runtime.InteropServices;
using System.Windows.Interop;
using WinUILib;

namespace SimpleWpfXamlIslandApp
{
    public class WinUIControlHost : HwndHost
    {
        int hostHeight, hostWidth;

        Microsoft.UI.Xaml.Hosting.DesktopWindowXamlSource _xamlSource;


        public WinUIControlHost(double height, double width)
        {
            hostHeight = (int)height;
            hostWidth = (int)width;
            _xamlSource = new Microsoft.UI.Xaml.Hosting.DesktopWindowXamlSource();
        }

        protected override HandleRef BuildWindowCore(HandleRef hwndParent)
        {
            var id = new Microsoft.UI.WindowId((ulong)hwndParent.Handle);
            _xamlSource.Initialize(id);

            InitIslandSampleCode();
            //_xamlSource.SiteBridge.ResizePolicy = Microsoft.UI.Content.ContentSizePolicy.ResizeContentToParentWindow;
            //_xamlSource.SiteBridge.Show();

            return new HandleRef(null, (nint)_xamlSource.SiteBridge.WindowId.Value);
        }

        void InitIslandSampleCode()
        {
            var grid = new Microsoft.UI.Xaml.Controls.Grid();
            grid.Background = new Microsoft.UI.Xaml.Media.SolidColorBrush(Colors.LightGray);
            grid.RowDefinitions.Add(new Microsoft.UI.Xaml.Controls.RowDefinition { Height = new Microsoft.UI.Xaml.GridLength(1, Microsoft.UI.Xaml.GridUnitType.Star) });
            grid.RowDefinitions.Add(new Microsoft.UI.Xaml.Controls.RowDefinition { Height = new Microsoft.UI.Xaml.GridLength(1, Microsoft.UI.Xaml.GridUnitType.Star) });
        //    grid.RowDefinitions.Add(new Microsoft.UI.Xaml.Controls.RowDefinition { Height = new Microsoft.UI.Xaml.GridLength(1, Microsoft.UI.Xaml.GridUnitType.Star) });

            var winUiButton = new MyCustomButton();
            winUiButton.Click += (s, e) =>
            {
                System.Windows.MessageBox.Show("Hello from WinUI!");
            };
            grid.Children.Add(winUiButton);

            //var customUserControl = new CustomUserControl();

            //grid.Children.Add(customUserControl);

            //Microsoft.UI.Xaml.Controls.Grid.SetRow(customUserControl, 1);

            var listView = new CustomListView {

                ItemsSource = new List<string> { "Item 1", "Item 2", "Item 3" },
            };

            Microsoft.UI.Xaml.Controls.Grid.SetRow(listView, 1);
            grid.Children.Add(listView);

            _xamlSource.Content = grid;
        }

        protected override void DestroyWindowCore(HandleRef hwnd)
        {

        }
    }
}
