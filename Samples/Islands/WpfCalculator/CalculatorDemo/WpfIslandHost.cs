using Microsoft.UI.Composition;
using Microsoft.UI.Content;
using System.Runtime.InteropServices;
using System.Windows.Interop;

namespace CalculatorDemo
{
    internal class WpfIslandHost : HwndHost
    {
        public WpfIslandHost(Compositor compositor)
        {
            _compositor = compositor;
        }

        public DesktopChildSiteBridge DesktopChildSiteBridge { get; private set; }

        protected override HandleRef BuildWindowCore(HandleRef hwndParent)
        {
            DesktopChildSiteBridge = Microsoft.UI.Content.DesktopChildSiteBridge.Create(
                _compositor,
                new Microsoft.UI.WindowId((ulong)hwndParent.Handle));

            return new HandleRef(null, (nint)DesktopChildSiteBridge.WindowId.Value);
        }

        protected override void DestroyWindowCore(HandleRef hwnd)
        {
            DesktopChildSiteBridge.Dispose();
            DesktopChildSiteBridge = null;
        }

        Microsoft.UI.Composition.Compositor _compositor;
    }
}