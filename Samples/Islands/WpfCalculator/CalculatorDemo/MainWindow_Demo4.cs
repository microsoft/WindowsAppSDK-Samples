// // Copyright (c) Microsoft. All rights reserved.
// // Licensed under the MIT license. See LICENSE file in the project root for full license information.

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
    }
}

#endif
