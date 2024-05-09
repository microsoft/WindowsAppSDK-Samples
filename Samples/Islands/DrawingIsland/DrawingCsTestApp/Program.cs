// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;

using Microsoft.UI;
using Microsoft.UI.Composition;
using Microsoft.UI.Content;
using Microsoft.UI.Dispatching;
using Microsoft.UI.Input;
using Microsoft.UI.Windowing;

using DrawingIslandComponents;

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

var siteBridge = DesktopChildSiteBridge.Create(compositor, window.Id);
siteBridge.ResizePolicy = ContentSizePolicy.ResizeContentToParentWindow;
siteBridge.Show();

if (args.Contains("Lottie"))
{
    // LottieIsland
    var lottie = LottieIslandScenario.CreateLottieIsland(compositor);
    siteBridge.Connect(lottie.Island);
}
else if (args.Contains("Duck"))
{
    // SceneNodeIsland
    var island = DuckScenario.CreateIsland(compositor);
    siteBridge.Connect(island);
}
else if (args.Contains("Helmet"))
{
    // SceneNodeIsland
    var island = HelmetScenario.CreateIsland(compositor);
    siteBridge.Connect(island);
}
else
{
    // DrawingIsland
    var drawing = new DrawingIsland(compositor);
    siteBridge.Connect(drawing.Island);
}

// Move initial focus to the island.
var focusNavigationHost = InputFocusNavigationHost.GetForSiteBridge(siteBridge);
focusNavigationHost.NavigateFocus(FocusNavigationRequest.Create(
    FocusNavigationReason.Programmatic));

queue.RunEventLoop();

controller.ShutdownQueue();
