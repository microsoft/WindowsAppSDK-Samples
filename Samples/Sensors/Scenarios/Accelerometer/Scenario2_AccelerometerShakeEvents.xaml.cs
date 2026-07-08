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

using Windows.Devices.Sensors;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario2_AccelerometerShakeEvents : Page
    {
        private Accelerometer _accelerometer;
        private ushort _shakeCount = 0;

        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public Scenario2_AccelerometerShakeEvents()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            _accelerometer = Accelerometer.GetDefault(AccelerometerHelper.ReadingType);
            if (_accelerometer != null)
            {
                rootPage.NotifyUser(AccelerometerHelper.ReadingType + " accelerometer ready", NotifyType.StatusMessage);
                ScenarioEnableButton.IsEnabled = true;
            }
            else
            {
                rootPage.NotifyUser(AccelerometerHelper.ReadingType + " accelerometer not found", NotifyType.ErrorMessage);
            }
        }

        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            if (ScenarioDisableButton.IsEnabled)
            {
                ScenarioDisable();
            }
        }

        /// <summary>
        /// This is the event handler for VisibilityChanged events. You would register for these notifications
        /// if handling sensor data when the app is not visible could cause unintended actions in the app.
        /// </summary>
        private void VisibilityChanged(object sender, WindowVisibilityChangedEventArgs e)
        {
            if (ScenarioDisableButton.IsEnabled)
            {
                if (e.Visible)
                {
                    // Re-enable sensor input
                    _accelerometer.Shaken += Shaken;
                }
                else
                {
                    // Disable sensor input
                    _accelerometer.Shaken -= Shaken;
                }
            }
        }

        /// <summary>
        /// This is the event handler for Shaken events.
        /// </summary>
        private void Shaken(object sender, AccelerometerShakenEventArgs e)
        {
            // CoreDispatcher is replaced by DispatcherQueue in WinUI 3.
            DispatcherQueue.TryEnqueue(() =>
            {
                _shakeCount++;
                ScenarioOutputText.Text = _shakeCount.ToString();
            });
        }

        /// <summary>
        /// This is the click handler for the 'Enable' button.
        /// </summary>
        private void ScenarioEnable()
        {
            App.MainWindow.VisibilityChanged += VisibilityChanged;
            _accelerometer.Shaken += Shaken;
            ScenarioEnableButton.IsEnabled = false;
            ScenarioDisableButton.IsEnabled = true;
        }

        /// <summary>
        /// This is the click handler for the 'Disable' button.
        /// </summary>
        private void ScenarioDisable()
        {
            App.MainWindow.VisibilityChanged -= VisibilityChanged;
            _accelerometer.Shaken -= Shaken;
            ScenarioEnableButton.IsEnabled = true;
            ScenarioDisableButton.IsEnabled = false;
        }
    }
}
