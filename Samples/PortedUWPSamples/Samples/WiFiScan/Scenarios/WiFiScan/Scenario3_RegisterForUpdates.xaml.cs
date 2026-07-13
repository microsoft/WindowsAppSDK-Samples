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
using Microsoft.UI.Xaml.Controls;
using Windows.Devices.WiFi;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Navigation;
using System.Collections.ObjectModel;

namespace SDKTemplate
{
    public sealed partial class Scenario3_RegisterForUpdates : Page
    {
        private WiFiAdapter firstAdapter;

        private MainPage rootPage = MainPage.Current;
        public ObservableCollection<WiFiNetworkDisplay> ResultCollection
        {
            get;
            private set;
        }

        public Scenario3_RegisterForUpdates()
        {
            this.InitializeComponent();
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            ResultCollection = new ObservableCollection<WiFiNetworkDisplay>();

            // RequestAccessAsync must have been called at least once by the app before using the API
            // Calling it multiple times is fine but not necessary
            // RequestAccessAsync must be called from the UI thread
            var access = await WiFiAdapter.RequestAccessAsync();
            if (access != WiFiAccessStatus.Allowed)
            {
                rootPage.NotifyUser("Access denied", NotifyType.ErrorMessage);
            }
            else
            {
                DataContext = this;

                var result = await Windows.Devices.Enumeration.DeviceInformation.FindAllAsync(WiFiAdapter.GetDeviceSelector());
                if (result.Count >= 1)
                {
                    firstAdapter = await WiFiAdapter.FromIdAsync(result[0].Id);
                    RegisterButton.IsEnabled = true;
                }
                else
                {
                    rootPage.NotifyUser("No WiFi Adapters detected on this machine", NotifyType.ErrorMessage);
                }
            }
        }

        private void FirstAdapter_AvailableNetworksChanged(WiFiAdapter sender, object args)
        {
            DisplayNetworkReport(sender.NetworkReport, sender);
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            if (firstAdapter != null)
            {
                firstAdapter.AvailableNetworksChanged -= FirstAdapter_AvailableNetworksChanged;
            }

            base.OnNavigatedFrom(e);
        }

        private void DisplayNetworkReport(WiFiNetworkReport report, WiFiAdapter adapter)
        {
            App.MainDispatcherQueue.TryEnqueue(async () =>
            {
                rootPage.NotifyUser(string.Format("Network Report Timestamp: {0}", report.Timestamp), NotifyType.StatusMessage);

                ResultCollection.Clear();

                foreach (var network in report.AvailableNetworks)
                {
                    var networkDisplay = new WiFiNetworkDisplay(network, adapter);
                    await networkDisplay.UpdateConnectivityLevel();
                    ResultCollection.Add(networkDisplay);
                }
            });
        }

        private void Button_Click_Register(object sender, RoutedEventArgs e)
        {
            firstAdapter.AvailableNetworksChanged += FirstAdapter_AvailableNetworksChanged;
            RegisterButton.IsEnabled = false;
            UnregisterButton.IsEnabled = true;
        }

        private void Button_Click_Unregister(object sender, RoutedEventArgs e)
        {
            firstAdapter.AvailableNetworksChanged -= FirstAdapter_AvailableNetworksChanged;
            UnregisterButton.IsEnabled = false;
            RegisterButton.IsEnabled = true;
        }
    }
}
