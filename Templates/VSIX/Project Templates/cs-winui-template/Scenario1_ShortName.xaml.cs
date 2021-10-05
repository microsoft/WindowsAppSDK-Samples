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

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

namespace $safeprojectname$
{
    public partial class Scenario1_ShortName : Page
    {
        public Scenario1_ShortName()
        {
            this.InitializeComponent();
        }

        public void NotifyUser(string strMessage, InfoBarSeverity severity, bool isOpen = true)
        {
            // If called from the UI thread, then update immediately.
            // Otherwise, schedule a task on the UI thread to perform the update.
            if (DispatcherQueue.HasThreadAccess)
            {
                UpdateStatus(strMessage, severity, isOpen);
            }
            else
            {
                DispatcherQueue.TryEnqueue(() =>
                {
                    UpdateStatus(strMessage, severity, isOpen);
                });
            }
        }
        private void UpdateStatus(string strMessage, InfoBarSeverity severity, bool isOpen)
        {
            infoBar.Message = strMessage;
            infoBar.IsOpen = isOpen;
            infoBar.Severity = severity;
        }

        private void SuccessMessage_Click(object sender, RoutedEventArgs e)
        {
            //rootPage.NotifyUser("Everything was ok!",InfoBarSeverity.Success);
            NotifyUser("Everything was ok!", InfoBarSeverity.Success);

        }

        private void ErrorMessage_Click(object sender, RoutedEventArgs e)
        {
            NotifyUser("Something went wrong.", InfoBarSeverity.Error);
        }

        private void InformationalMessage_Click(object sender, RoutedEventArgs e)
        {
            NotifyUser("This is the informational bar.", InfoBarSeverity.Informational);
        }

        private void ClearMessage_Click(object sender, RoutedEventArgs e)
        {
            NotifyUser("", InfoBarSeverity.Informational, false);
        }
    }
}
