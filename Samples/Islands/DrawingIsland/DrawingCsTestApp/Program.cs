using System;
﻿
using Microsoft.UI;
using Microsoft.UI.Composition;
using Microsoft.UI.Content;
using Microsoft.UI.Dispatching;
using Microsoft.UI.Input;
using Microsoft.UI.Windowing;

using DrawingIslandComponents;
using CommunityToolkit.WinAppSDK.LottieIsland;
using CommunityToolkit.WinAppSDK.LottieWinRT;
using CommunityToolkit.WinUI.Lottie;
using System.Diagnostics;

var controller = DispatcherQueueController.CreateOnCurrentThread();
var queue = controller.DispatcherQueue;

var window = AppWindow.Create();
window.AssociateWithDispatcherQueue(queue);
window.Closing +=(sender, args) =>
    {
        queue.EnqueueEventLoopExit();
    };

window.Title = "Drawing C# .NET TestApp";
window.Show();

var compositor = new Compositor();

// DrawingIsland
//var drawing = new DrawingIsland(compositor);

//var siteBridge = DesktopChildSiteBridge.Create(compositor, window.Id);
//siteBridge.ResizePolicy = ContentSizePolicy.ResizeContentToParentWindow;
//siteBridge.Show();
//siteBridge.Connect(drawing.Island);

// LottieIsland
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

var siteBridge = DesktopChildSiteBridge.Create(compositor, window.Id);
siteBridge.ResizePolicy = ContentSizePolicy.ResizeContentToParentWindow;
siteBridge.Show();
siteBridge.Connect(lottieIsland.Island);

queue.RunEventLoop();

controller.ShutdownQueue();
