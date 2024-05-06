using CommunityToolkit.WinAppSDK.LottieIsland;
using CommunityToolkit.WinAppSDK.LottieWinRT;
using Microsoft.UI.Composition;
using Microsoft.UI.Content;

class LottieIslandScenario
{
    public static void CreateLottieIsland(Compositor compositor, DesktopChildSiteBridge siteBridge)
    {
        var lottieIsland = LottieContentIsland.Create(compositor);
        var lottieVisualSource = LottieVisualSourceWinRT.CreateFromString("ms-appx:///Assets/LottieLogo1.json");

        if (lottieVisualSource != null)
        {
            lottieVisualSource.AnimatedVisualInvalidated += (sender, args) =>
            {
                object? diagnostics = null;
                IAnimatedVisualFrameworkless? animatedVisual = lottieVisualSource.TryCreateAnimatedVisual(compositor, out diagnostics);
                if (animatedVisual != null && lottieIsland != null)
                {
                    compositor.DispatcherQueue.TryEnqueue(() =>
                    {
                        lottieIsland.AnimatedVisual = animatedVisual;
                        lottieIsland.PlayAsync(0, 1, true);
                    });
                }
            };
        }

        siteBridge.Connect(lottieIsland.Island);
    }
}