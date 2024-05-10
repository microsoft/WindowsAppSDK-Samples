// // Copyright (c) Microsoft. All rights reserved.
// // Licensed under the MIT license. See LICENSE file in the project root for full license information.

#if true // Demo3_Step1_AddWasdk
using Microsoft.UI.Dispatching;
#endif
using System.Windows;

namespace CalculatorDemo
{
    /// <summary>
    ///     Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
#if true // Demo3_Step1_AddWasdk
        // Many WinAppSDK APIs require a DispatcherQueue to be running on the thread.  We'll start one when the app starts up
        // and shut it down when the app is finished.
        protected override void OnStartup(StartupEventArgs e)
        {
            _dispatcherQueueController = DispatcherQueueController.CreateOnCurrentThread();
            base.OnStartup(e);
        }

        protected override void OnExit(ExitEventArgs e)
        {
            base.OnExit(e);
            _dispatcherQueueController?.ShutdownQueue();
            _dispatcherQueueController = null;
        }

        DispatcherQueueController? _dispatcherQueueController;
#endif
    }

}