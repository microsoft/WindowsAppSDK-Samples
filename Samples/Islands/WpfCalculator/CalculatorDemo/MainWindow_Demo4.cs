// // Copyright (c) Microsoft. All rights reserved.
// // Licensed under the MIT license. See LICENSE file in the project root for full license information.

using CommunityToolkit.WinAppSDK.LottieIsland;
using System.Windows;

#if true // Demo3_Step2_AddCompact

namespace CalculatorDemo
{
    public sealed partial class MainWindow : Window
    {
        private Microsoft.UI.Composition.Compositor _compositor = new();

        private void CreateLottieIslandMenuItem_Click(object sender, RoutedEventArgs e)
        {
            var wpfIslandHost = new WpfIslandHost(_compositor);
            var lottieContentIsland = LottieIslandScenario.CreateLottieIsland(_compositor);

            DisplayAreaBorder.Child = wpfIslandHost;
            wpfIslandHost.DesktopChildSiteBridge.Connect(lottieContentIsland.Island);
        }
    }
}

#endif
