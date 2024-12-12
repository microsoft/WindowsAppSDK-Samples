// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Navigation;
using Microsoft.Windows.ApplicationModel.Background;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.ApplicationModel.Background;
using Windows.Foundation;
using Windows.Foundation.Collections;
using WinRT;
using WinRT.Interop;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace BackgroundTaskBuilder
{
    /// <summary>
    /// An empty window that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainWindow : Window
    {
        public MainWindow()
        {
            this.InitializeComponent();
        }
        private void unregisterTasks()
        {
            var allRegistrations = BackgroundTaskRegistration.AllTasks;
            foreach (var taskPair in allRegistrations)
            {
                IBackgroundTaskRegistration task = taskPair.Value;
                task.Unregister(true);
            }
        }

        private void myButton_Click(object sender, RoutedEventArgs e)
        {
            myButton.Content = "Registered Task for TimeZone Change";
            myButton.IsEnabled = false;
            unregisterTasks();
            registerTimeZoneChangedTask();
        }

        private void registerTimeZoneChangedTask()
        {
            // Using the WinAppSDK BackgroundTaskBuilder API to register a background task
            var taskBuilder = new Microsoft.Windows.ApplicationModel.Background.BackgroundTaskBuilder
            {
                Name = "TimeZoneChangedTask"
            };
            taskBuilder.SetTaskEntryPointClsid(typeof(BackgroundTask).GUID);
            taskBuilder.SetTrigger(new SystemTrigger(SystemTriggerType.TimeZoneChange, false));
            BackgroundTaskRegistration task = taskBuilder.Register();
        }
    }
}
