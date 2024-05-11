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

var window = AppWindow.Create();
window.AssociateWithDispatcherQueue(queue);
window.Closing +=(sender, args) =>
    {
        queue.EnqueueEventLoopExit();
    };

window.Title = "Drawing C# .NET TestApp";
window.Show();

#region ...
//var compositor = new Compositor();

//var siteBridge = DesktopChildSiteBridge.Create(compositor, window.Id);
//siteBridge.ResizePolicy = ContentSizePolicy.ResizeContentToParentWindow;
//siteBridge.Show();

//var drawing = new DrawingIsland(compositor);
//siteBridge.Connect(drawing.Island);

#region ...
//var lottie = LottieIslandScenario.CreateLottieIsland(compositor);
//siteBridge.Connect(lottie.Island);

#region ...
//var island = DuckScenario.CreateIsland(compositor);
//siteBridge.Connect(island);

#region ...
//var island = HelmetScenario.CreateIsland(compositor);
//siteBridge.Connect(island);
#endregion

#endregion
#endregion

//var focusNavigationHost = InputFocusNavigationHost.GetForSiteBridge(siteBridge);
//focusNavigationHost.NavigateFocus(FocusNavigationRequest.Create(
//    FocusNavigationReason.Programmatic));
#endregion

queue.RunEventLoop();

controller.ShutdownQueue();
