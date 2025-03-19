// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.


using System;
using System.IO; // Path
using System.Collections.Generic; // Queue
using Windows.System.Threading; // ThreadPoolTimer
using System.Runtime.InteropServices; // Guid, RegistrationServices
using Windows.ApplicationModel.Background; // IBackgroundTask
using Windows.UI.Notifications; // ToastNotificationManager, ToastTemplateType, ToastNotification

namespace BackgroundTaskBuilder
{
    // Background task implementation.
    // {87654321-1234-1234-1234-1234567890AE} is GUID to register this task with BackgroundTaskBuilder. Generate a random GUID before implementing.
    [Guid("87654321-1234-1234-1234-1234567890AA")]
    public partial class BackgroundTask : IBackgroundTask
    {
        /// <summary>
        /// This method is the main entry point for the background task. The system will believe this background task
        /// is complete when this method returns.
        /// </summary>
        [MTAThread]
        public void Run(IBackgroundTaskInstance taskInstance)
        {
            // Get deferral to indicate not to kill the background task process as soon as the Run method returns
            _deferral = taskInstance.GetDeferral();
            // Wire the cancellation handler.
            taskInstance.Canceled += this.OnCanceled;

            // Set the progress to indicate this task has started
            taskInstance.Progress = 0;

            _periodicTimer = ThreadPoolTimer.CreatePeriodicTimer(new TimerElapsedHandler(PeriodicTimerCallback), TimeSpan.FromSeconds(1));
        }

        // Simulate the background task activity.
        private void PeriodicTimerCallback(ThreadPoolTimer timer)
        {
            if ((_cancelRequested == false) && (_progress < 100))
            {
                _progress += 10;
            }
            else
            {
                if (_cancelRequested) _progress = -1;
                if (_periodicTimer != null) _periodicTimer.Cancel();

                // Indicate that the background task has completed.
                if (_deferral != null) _deferral.Complete();
            }

            BackgroundTaskBuilder.MainWindow.taskStatus(_progress);
        }

        /// <summary>
        /// This method is signaled when the system requests the background task be canceled. This method will signal
        /// to the Run method to clean up and return.
        /// </summary>
        [MTAThread]
        public void OnCanceled(IBackgroundTaskInstance taskInstance, BackgroundTaskCancellationReason cancellationReason)
        {
            // Handle cancellation operations and flag the task to end
            _cancelRequested = true;
        }

        private BackgroundTaskDeferral? _deferral = null;
        private volatile bool _cancelRequested = false;
        private ThreadPoolTimer? _periodicTimer = null;
        private int _progress = 0;
    }
}
