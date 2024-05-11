// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

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

#region Example 1
var drawing = new DrawingIsland(compositor);
siteBridge.Connect(drawing.Island);
#endregion

#region Example 2
//var lottie = LottieIslandScenario.CreateLottieIsland(compositor);
//siteBridge.Connect(lottie.Island);
#endregion

#region Example 3
//var island = DuckScenario.CreateIsland(compositor);
//siteBridge.Connect(island);
#endregion

#region Example 4
//var island = HelmetScenario.CreateIsland(compositor);
//siteBridge.Connect(island);
#endregion

// Move initial focus to the island.
var focusNavigationHost = InputFocusNavigationHost.GetForSiteBridge(siteBridge);
focusNavigationHost.NavigateFocus(FocusNavigationRequest.Create(
    FocusNavigationReason.Programmatic));

queue.RunEventLoop();

controller.ShutdownQueue();
