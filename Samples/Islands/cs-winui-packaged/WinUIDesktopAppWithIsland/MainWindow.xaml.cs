using System;
using System.Collections.Generic;
using System.Numerics;
using CommunityToolkit.WinAppSDK.LottieIsland;
using CommunityToolkit.WinAppSDK.LottieWinRT;
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


        public void SetupLottie()
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

            LottieContentIsland lottieIsland = CreateLottieIsland(placementVisual.Compositor);

            childContentLink.Connect(lottieIsland.Island);

            _keepAlive.Add(childContentLink);
            _keepAlive.Add(lottieIsland);
        }

        public static LottieContentIsland CreateLottieIsland(Compositor compositor)
        {
            var lottieIsland = LottieContentIsland.Create(compositor);
            var lottieVisualSource = LottieVisualSourceWinRT.CreateFromString(
                "ms-appx:///Assets/LottieLogo1.json");

            if (lottieVisualSource != null)
            {
                lottieVisualSource.AnimatedVisualInvalidated += (sender, args) =>
                {
                    object? diagnostics = null;
                    IAnimatedVisualFrameworkless? animatedVisual =
                        lottieVisualSource.TryCreateAnimatedVisual(compositor, out diagnostics);

                    if (animatedVisual != null)
                    {
                        // This callback comes back on a different thread, so set the AnimatedVisual on
                        // the UI thread
                        compositor.DispatcherQueue.TryEnqueue(async () =>
                        {
                            lottieIsland.AnimatedVisual = animatedVisual;
                            await lottieIsland.PlayAsync(0, 1, true);
                        });
                    }
                };
            }

            return lottieIsland;
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            SetupHelmet();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            SetupLottie();
        }
    }
}
