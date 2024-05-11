// // Copyright (c) Microsoft. All rights reserved.
// // Licensed under the MIT license. See LICENSE file in the project root for full license information.

using CommunityToolkit.WinAppSDK.LottieIsland;
using System.Windows;

#if true // Demo3_Step2_AddCompact

namespace CalculatorDemo
{
    public sealed partial class MainWindow : Window
    {
        private Microsoft.UI.Composition.Compositor _compositor = new Microsoft.UI.Composition.Compositor();
        private void CreateDrawingIslandMenuItem_Click(object sender, RoutedEventArgs e)
        {
            var wpfIslandHost = new WpfIslandHost(_compositor);
            var drawingIsland = new DrawingIslandComponents.DrawingIsland(_compositor);

            // After this, the WpfIslandHost will be live, and the DesktopChildSiteBridge will be available.
            DisplayAreaBorder.Child = wpfIslandHost;

            wpfIslandHost.DesktopChildSiteBridge.Connect(drawingIsland.Island);
        }

        private void CreateLottieIslandMenuItem_Click(object sender, RoutedEventArgs e)
        {
            var wpfIslandHost = new WpfIslandHost(_compositor);
            _lottieContentIsland = LottieIslandScenario.CreateLottieIsland(_compositor);

            // After this, the WpfIslandHost will be live, and the DesktopChildSiteBridge will be available.
            DisplayAreaBorder.Child = wpfIslandHost;

            wpfIslandHost.DesktopChildSiteBridge.Connect(_lottieContentIsland.Island);
        }

        private async void CreateSceneNodeIslandMenuItem_Click(object sender, RoutedEventArgs e)
        {
            var wpfIslandHost = new WpfIslandHost(_compositor);
            var helmetIsland = await HelmetScenario.CreateIsland(_compositor);

            // After this, the WpfIslandHost will be live, and the DesktopChildSiteBridge will be available.
            DisplayAreaBorder.Child = wpfIslandHost;

            wpfIslandHost.DesktopChildSiteBridge.Connect(helmetIsland);
        }

        LottieContentIsland _lottieContentIsland;
    }
}

#endif
