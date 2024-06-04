using System.Collections.Generic;
using System.Numerics;
using Microsoft.UI.Composition;
using Microsoft.UI.Content;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Hosting;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace WinUIDesktopAppWithIsland
{
    /// <summary>
    /// An empty window that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainWindow : Window
    {
        public MainWindow()
        {
            this.InitializeComponent();
        }

        List<object> _keepAlive = new List<object>();
        int idx = 0;

        Microsoft.UI.Xaml.Shapes.Rectangle GetNextHostElement()
        {
            if (idx < _rectanglePanel.Children.Count)
            {
                return ((Microsoft.UI.Xaml.Shapes.Rectangle)_rectanglePanel.Children[idx++]);
            }

            return null;
        }

        public async void SetupHelmet()
        {
            ContentIsland parentIsland = this.Content.XamlRoot.TryGetContentIsland();
            ContainerVisual placementVisual = Compositor.CreateContainerVisual();
            ChildContentLink childContentLink = ChildContentLink.Create(parentIsland, placementVisual);

            Microsoft.UI.Xaml.Shapes.Rectangle rect = GetNextHostElement();
            if (rect == null)
            {
                return;
            }

            ElementCompositionPreview.SetElementChildVisual(rect, placementVisual);
            Vector2 size = rect.ActualSize;

            placementVisual.Size = size;
            childContentLink.ActualSize = size;

            ContentIsland helmetIsland = await HelmetScenario.CreateIsland(placementVisual.Compositor);

            childContentLink.Connect(helmetIsland);

            _keepAlive.Add(childContentLink);
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            SetupHelmet();
        }
    }
}
