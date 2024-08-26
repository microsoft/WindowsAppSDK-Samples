// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#region Using directives
using Microsoft.UI.Composition;
using Microsoft.UI.Content;
using Microsoft.UI.Dispatching;
using Microsoft.UI.Input;
using Microsoft.UI.Windowing;

using DrawingIslandComponents;
#endregion

var controller = DispatcherQueueController.CreateOnCurrentThread();
var queue = controller.DispatcherQueue;

// Associate the AppWindow's lifetime with the DispatcherQueue to automatically close on exit.
var window = AppWindow.Create();
window.AssociateWithDispatcherQueue(queue);
window.Closing +=(sender, args) =>
    {
        // Ensure the DispatcherQueue exits the event loop on close.
        queue.EnqueueEventLoopExit();
    };

window.Title = "Drawing C# .NET TestApp";
window.Show();

#region ...
// Create a ContentSiteBridge and connect Island content into it.
var compositor = new Compositor();
var siteBridge = DesktopChildSiteBridge.Create(compositor, window.Id);
siteBridge.ResizePolicy = ContentSizePolicy.ResizeContentToParentWindow;
siteBridge.Show();

//var drawing = new DrawingIsland(compositor);
//siteBridge.Connect(drawing.Island);

#region ...
//var lottie = LottieIslandScenario.CreateLottieIsland(compositor);
//siteBridge.Connect(lottie.Island);

#region ...
var island = HelmetScenario.CreateIsland(compositor);
siteBridge.Connect(island);

#endregion
#endregion

// Move initial focus to the ContentIsland.
var focusNavigationHost = InputFocusNavigationHost.GetForSiteBridge(siteBridge);
focusNavigationHost.NavigateFocus(FocusNavigationRequest.Create(
    FocusNavigationReason.Programmatic));
#endregion

queue.RunEventLoop();

controller.ShutdownQueue();
