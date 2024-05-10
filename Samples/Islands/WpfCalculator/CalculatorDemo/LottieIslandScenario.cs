using System;
using Microsoft.UI.Composition;
using CommunityToolkit.WinAppSDK.LottieIsland;
using CommunityToolkit.WinAppSDK.LottieWinRT;

class LottieIslandScenario
{
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
}