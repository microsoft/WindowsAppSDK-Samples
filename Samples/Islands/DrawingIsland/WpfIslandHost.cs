using DrawingIslandComponents;
using Microsoft.UI.Content;
using Microsoft.UI.Dispatching;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Numerics;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Interop;
using Windows.UI;

namespace CalculatorDemo
{
    internal class WpfIslandHost : HwndHost
    {
        protected override HandleRef BuildWindowCore(HandleRef hwndParent)
        {
            _compositor = new Microsoft.UI.Composition.Compositor();

            _desktopChildSiteBridge = Microsoft.UI.Content.DesktopChildSiteBridge.Create(
                _compositor,
                new Microsoft.UI.WindowId((ulong)hwndParent.Handle));

            //SetupGreenRectangleIsland();
            SetupDrawingIsland();

            return new HandleRef(null, (nint)_desktopChildSiteBridge.WindowId.Value);
        }

        protected override void DestroyWindowCore(HandleRef hwnd)
        {
            _desktopChildSiteBridge?.Dispose();
            _desktopChildSiteBridge = null;
        }

        private void SetupDrawingIsland()
        {
            _desktopChildSiteBridge.Connect(new DrawingIsland(_compositor).Island);
        }

        private void SetupGreenRectangleIsland()
        {
            _root = _compositor.CreateContainerVisual();

            var child = _compositor.CreateSpriteVisual();
            child.Brush = _compositor.CreateColorBrush(Color.FromArgb(0xFF, 0x00, 0xCC, 0x00));
            child.Size = new Vector2(400, 400);

            _root.Children.InsertAtTop(child);

            _island = ContentIsland.Create(_root);
            _island.RequestSize(new Vector2(200, 200));

            _desktopChildSiteBridge.Connect(_island);
        }

        Microsoft.UI.Composition.Compositor? _compositor;
        Microsoft.UI.Composition.ContainerVisual? _root;
        Microsoft.UI.Content.DesktopChildSiteBridge? _desktopChildSiteBridge;
        Microsoft.UI.Content.ContentIsland? _island;
    }
}
