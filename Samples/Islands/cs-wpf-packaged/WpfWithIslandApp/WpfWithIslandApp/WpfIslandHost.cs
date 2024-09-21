using CommunityToolkit.WinAppSDK.LottieIsland;
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

namespace WpfWithIslandApp
{
    class WpfIslandHost : HwndHost
    {
        protected override HandleRef BuildWindowCore(HandleRef hwndParent)
        {
            _compositor = new Microsoft.UI.Composition.Compositor();

            _desktopChildSiteBridge = Microsoft.UI.Content.DesktopChildSiteBridge.Create(
                _compositor,
                new Microsoft.UI.WindowId((ulong)hwndParent.Handle));

            //SetupGreenRectangleIsland();
            SetupLottieIsland();

            return new HandleRef(null, (nint)_desktopChildSiteBridge.WindowId.Value);
        }

        protected override void DestroyWindowCore(HandleRef hwnd)
        {
            
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

        private void SetupLottieIsland()
        {
            // Create the LottieIsland
            _lottieContentIsland = LottieContentIsland.Create(this._compositor);
            if (_lottieContentIsland != null)
            {
                Debug.WriteLine("LottieContentIsland created!");
            }
            else
            {
                Debug.WriteLine("LottieContentIsland creation failed :(");
            }

            // Load the Lottie from JSON
            _lottieVisualSource = LottieWinRT.LottieVisualSourceWinRT.CreateFromString("ms-appx:///Assets/LottieLogo1.json");
            if (_lottieVisualSource != null)
            {
                _lottieVisualSource.AnimatedVisualInvalidated += LottieVisualSource_AnimatedVisualInvalidated;
            }
            else
            {
                Debug.WriteLine("Failed to load LottieVisualSourceWinRT from file");
            }

            _desktopChildSiteBridge.Connect(_lottieContentIsland.Island);
        }

        private void LottieVisualSource_AnimatedVisualInvalidated(object? sender, object? e)
        {
            Debug.Assert(_lottieVisualSource != null);
            {
                object? diagnostics = null;
                IAnimatedVisualFrameworkless? animatedVisual = _lottieVisualSource.TryCreateAnimatedVisual(this._compositor, out diagnostics);
                if (animatedVisual != null)
                {
                    // Set Lottie as the content of the ContentIsland
                    if (_lottieContentIsland != null)
                    {
                        _lottieContentIsland.AnimatedVisualSource = _lottieVisualSource;
                    }
                    // Testing accessing a property through the WinRT component
                    Debug.WriteLine("Lottie duration: " + animatedVisual.Duration);
                }
                else
                {
                    Debug.WriteLine("Visual creation failed");
                }
            }
        }

        Microsoft.UI.Composition.Compositor? _compositor;
        Microsoft.UI.Composition.ContainerVisual? _root;
        Microsoft.UI.Content.DesktopChildSiteBridge? _desktopChildSiteBridge;
        Microsoft.UI.Content.ContentIsland? _island;

        private LottieWinRT.LottieVisualSourceWinRT? _lottieVisualSource;
        private LottieContentIsland? _lottieContentIsland;
    }
}
