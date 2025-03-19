// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.


using System;
using System.IO; // Path
using System.Threading; // EventWaitHandle
using System.Collections.Generic; // Queue
using System.Runtime.InteropServices; // Guid, RegistrationServices
using Windows.ApplicationModel.Background; // IBackgroundTask
using Windows.UI.Notifications; // ToastNotificationManager, ToastTemplateType, ToastNotification

namespace BackgroundTaskBuilder
{
    // Background task implementation.
    // {87654321-1234-1234-1234-1234567890AE} is GUID to register this task with BackgroundTaskBuilder. Generate a random GUID before implementing.
    [Guid("87654321-1234-1234-1234-1234567890AE")]
    public partial class BackgroundTask : IBackgroundTask, IDisposable
    {
        private bool disposed = false;

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (!disposed)
            {
                if (disposing)
                {
                    // Stop the server when the background task is disposed.
                    BackgroundTaskBuilder.Program.SignalExit();
                }
                disposed = true;
            }
        }

        ~BackgroundTask()
        {
            Dispose(false);
        }

        /// <summary>
        /// This method is the main entry point for the background task. The system will believe this background task
        /// is complete when this method returns.
        /// </summary>
        [MTAThread]
        public void Run(IBackgroundTaskInstance taskInstance)
        {
            // Wire the cancellation handler.
            taskInstance.Canceled += this.OnCanceled;

            // Set the progress to indicate this task has started
            taskInstance.Progress = 10;

            // Create the toast notification content
            var toastXml = ToastNotificationManager.GetTemplateContent(ToastTemplateType.ToastText01);
            var toastTextElements = toastXml.GetElementsByTagName("text");
            toastTextElements[0].AppendChild(toastXml.CreateTextNode("C# Background task executed"));

            // Create the toast notification
            var toast = new ToastNotification(toastXml);

            // Show the toast notification
            ToastNotificationManager.CreateToastNotifier().Show(toast);
        }

        /// <summary>
        /// This method is signaled when the system requests the background task be canceled. This method will signal
        /// to the Run method to clean up and return.
        /// </summary>
        [MTAThread]
        public void OnCanceled(IBackgroundTaskInstance taskInstance, BackgroundTaskCancellationReason cancellationReason)
        {
            // Unregister the task when the task is destroyed.
            BackgroundTaskBuilder.Program.SignalExit();
        }
    }
}
