//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.UI.Dispatching;
using Microsoft.UI.Xaml;
using Microsoft.Windows.AppLifecycle;

namespace SDKTemplate
{
    /// <summary>
    /// Custom application entry point.
    ///
    /// UWP apps are single-instanced, so opening a registered file or protocol simply raises
    /// <c>OnFileActivated</c>/<c>OnActivated</c> on the already-running app. WinUI 3 desktop apps
    /// are multi-instanced by default, so to reproduce that behaviour we must decide - before any
    /// window is created - whether this activation should be redirected to an existing instance.
    ///
    /// This replaces the auto-generated Main (the project defines DISABLE_XAML_GENERATED_MAIN).
    /// </summary>
    public static class Program
    {
        // A single, constant key means every activation (launch, file, protocol) funnels into
        // one instance, matching the UWP single-instance behaviour.
        private const string InstanceKey = "AssociationLaunchingMainInstance";

        [STAThread]
        static void Main(string[] args)
        {
            WinRT.ComWrappersSupport.InitializeComWrappers();

            bool isRedirect = DecideRedirection();
            if (!isRedirect)
            {
                Application.Start((p) =>
                {
                    var context = new DispatcherQueueSynchronizationContext(
                        DispatcherQueue.GetForCurrentThread());
                    SynchronizationContext.SetSynchronizationContext(context);
                    new App();
                });
            }
        }

        /// <summary>
        /// Registers this process under a shared key. If another instance already owns the key,
        /// the current activation is redirected to it and this instance exits.
        /// </summary>
        private static bool DecideRedirection()
        {
            bool isRedirect = false;

            AppInstance keyInstance = AppInstance.FindOrRegisterForKey(InstanceKey);

            if (keyInstance.IsCurrent)
            {
                // We are the primary instance. Listen for activations redirected here from other
                // instances (e.g. the user opens another .alsdk file while we are already running).
                keyInstance.Activated += OnActivated;
            }
            else
            {
                // Another instance already owns the key. Forward our activation arguments to it
                // and let this instance terminate.
                isRedirect = true;
                AppActivationArguments args = AppInstance.GetCurrent().GetActivatedEventArgs();
                RedirectActivationTo(args, keyInstance);
            }

            return isRedirect;
        }

        private static void OnActivated(object sender, AppActivationArguments args)
        {
            // A redirected activation arrives on a background thread; hand it to the app, which
            // will marshal to the UI thread and bring the existing window forward.
            App.HandleRedirectedActivation(args);
        }

        /// <summary>
        /// Performs the redirection on a worker thread and blocks (without pumping the message
        /// loop) until it completes.
        /// </summary>
        private static void RedirectActivationTo(AppActivationArguments args, AppInstance keyInstance)
        {
            var redirectSemaphore = new Semaphore(0, 1);
            Task.Run(() =>
            {
                keyInstance.RedirectActivationToAsync(args).AsTask().Wait();
                redirectSemaphore.Release();
            });
            redirectSemaphore.WaitOne();
        }
    }
}
