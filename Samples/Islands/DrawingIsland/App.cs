// // Copyright (c) Microsoft. All rights reserved.
// // Licensed under the MIT license. See LICENSE file in the project root for full license information.

using System.Windows;
using Microsoft.UI.Dispatching;

namespace CalculatorDemo
{
    /// <summary>
    ///     Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        protected override void OnStartup(StartupEventArgs e)
        {
            _dispacherQueueController = DispatcherQueueController.CreateOnCurrentThread();
            base.OnStartup(e);
        }

        protected override void OnExit(ExitEventArgs e)
        {
            base.OnExit(e);
            _dispacherQueueController?.ShutdownQueue();
            _dispacherQueueController = null;
        }

        DispatcherQueueController? _dispacherQueueController;
    }
}