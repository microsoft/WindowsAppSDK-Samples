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
var drawing = new DrawingIsland(compositor);

var siteBridge = DesktopChildSiteBridge.Create(compositor, window.Id);
siteBridge.ResizePolicy = ContentSizePolicy.ResizeContentToParentWindow;
siteBridge.Show();
siteBridge.Connect(drawing.Island);

// Move initial focus to the island.
var focusNavigationHost = InputFocusNavigationHost.GetForSiteBridge(siteBridge);
focusNavigationHost.NavigateFocus(FocusNavigationRequest.Create(
    FocusNavigationReason.Programmatic));

queue.RunEventLoop();

controller.ShutdownQueue();
