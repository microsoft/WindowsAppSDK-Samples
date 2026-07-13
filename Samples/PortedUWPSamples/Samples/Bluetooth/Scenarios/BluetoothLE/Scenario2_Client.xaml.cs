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
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;
using Windows.Devices.Bluetooth;
using Windows.Devices.Bluetooth.GenericAttributeProfile;
using Windows.Devices.Enumeration;
using Windows.Security.Cryptography;
using Windows.Storage.Streams;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Navigation;

namespace SDKTemplate
{
    // This scenario connects to the device selected in the "Discover
    // GATT Servers" scenario and communicates with it.
    // Note that this scenario is rather artificial because it communicates
    // with an unknown service with unknown characteristics.
    // In practice, your app will be interested in a specific service with
    // a specific characteristic.
    public sealed partial class Scenario2_Client : Page
    {
        private MainPage rootPage = MainPage.Current;

        private BluetoothLEDevice bluetoothLeDevice = null;
        private GattCharacteristic selectedCharacteristic = null;

        // Only one registered characteristic at a time.
        private GattCharacteristic registeredCharacteristic = null;

        #region UI Code
        public Scenario2_Client()
        {
            InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            SelectedDeviceRun.Text = BluetoothLEShared.SelectedBleDeviceName;
            if (string.IsNullOrEmpty(BluetoothLEShared.SelectedBleDeviceId))
            {
                ConnectButton.IsEnabled = false;
            }
        }

        protected override async void OnNavigatedFrom(NavigationEventArgs e)
        {
            await ClearBluetoothLEDeviceAsync();
        }
        #endregion

        #region Enumerating Services
        private async Task ClearBluetoothLEDeviceAsync()
        {
            // Capture the characteristic we want to unregister, in case the user changes it during the await.
            GattCharacteristic characteristic = registeredCharacteristic;
            registeredCharacteristic = null;

            if (characteristic != null)
            {
                // Clear the CCCD from the remote device so we stop receiving notifications
                GattCommunicationStatus result = await characteristic.WriteClientCharacteristicConfigurationDescriptorAsync(GattClientCharacteristicConfigurationDescriptorValue.None);
                if (result != GattCommunicationStatus.Success)
                {
                    // Even if we are unable to unsubscribe, continue with the rest of the cleanup.
                    rootPage.NotifyUser("Error: Unable to unsubscribe from notifications.", NotifyType.ErrorMessage);
                }

                characteristic.ValueChanged -= Characteristic_ValueChanged;
            }
            bluetoothLeDevice?.Dispose();
            bluetoothLeDevice = null;
        }

        private async void ConnectButton_Click(object sender, RoutedEventArgs e)
        {
            ConnectButton.IsEnabled = false;
            ServiceList.Visibility = Visibility.Collapsed;

            await ClearBluetoothLEDeviceAsync();

            // BT_Code: BluetoothLEDevice.FromIdAsync must be called from a UI thread because it may prompt for consent.
            bluetoothLeDevice = await BluetoothLEDevice.FromIdAsync(BluetoothLEShared.SelectedBleDeviceId);

            if (bluetoothLeDevice != null)
            {
                // Note: BluetoothLEDevice.GattServices property will return an empty list for unpaired devices. For all uses we recommend using the GetGattServicesAsync method.
                // BT_Code: GetGattServicesAsync returns a list of all the supported services of the device (even if it's not paired to the system).
                // If the services supported by the device are expected to change during BT usage, subscribe to the GattServicesChanged event.
                GattDeviceServicesResult result = null;
                try
                {
                    result = await bluetoothLeDevice.GetGattServicesAsync(BluetoothCacheMode.Uncached);
                }
                catch (TaskCanceledException)
                {
                    // The bluetoothLeDevice was disposed by OnNavigatedFrom while GetGattServicesAsync was running.
                    ConnectButton.IsEnabled = true;
                    return;
                }

                if (result.Status == GattCommunicationStatus.Success)
                {
                    IReadOnlyList<GattDeviceService> services = result.Services;
                    rootPage.NotifyUser(String.Format("Found {0} services", services.Count), NotifyType.StatusMessage);
                    foreach (var service in services)
                    {
                        ServiceList.Items.Add(new ComboBoxItem { Content = DisplayHelpers.GetServiceName(service), Tag = service });
                    }
                    ConnectButton.Visibility = Visibility.Collapsed;
                    ServiceList.Visibility = Visibility.Visible;
                }
                else
                {
                    rootPage.NotifyUser($"Error: {Utilities.FormatGattCommunicationStatus(result.Status, result.ProtocolError)}", NotifyType.ErrorMessage);
                }
            }
            else
            {
                rootPage.NotifyUser("Unable to find device. Maybe it isn't connected any more.", NotifyType.ErrorMessage);
            }
            ConnectButton.IsEnabled = true;
        }
        #endregion

        #region Enumerating Characteristics
        private async void ServiceList_SelectionChanged(object sender, RoutedEventArgs e)
        {
            CharacteristicList.Items.Clear();
            CharacteristicList.Visibility = Visibility.Collapsed;
            RemoveValueChangedHandler();

            var service = (GattDeviceService)((ComboBoxItem)ServiceList.SelectedItem)?.Tag;
            if (service == null)
            {
                rootPage.NotifyUser("No service selected", NotifyType.ErrorMessage);
                return;
            }

            IReadOnlyList<GattCharacteristic> characteristics = null;
            try
            {
                // Ensure we have access to the device.
                DeviceAccessStatus accessStatus = await service.RequestAccessAsync();
                if (accessStatus != DeviceAccessStatus.Allowed)
                {
                    // Not granted access
                    rootPage.NotifyUser("Error accessing service.", NotifyType.ErrorMessage);
                    return;
                }

                // BT_Code: Get all the child characteristics of a service. Use the cache mode to specify uncached characteristics only
                // and the new Async functions to get the characteristics of unpaired devices as well.
                GattCharacteristicsResult result = await service.GetCharacteristicsAsync(BluetoothCacheMode.Uncached);
                if (result.Status != GattCommunicationStatus.Success)
                {
                    rootPage.NotifyUser($"Error accessing service: {Utilities.FormatGattCommunicationStatus(result.Status, result.ProtocolError)}", NotifyType.ErrorMessage);
                    return;
                }
                characteristics = result.Characteristics;
            }
            catch (Exception ex)
            {
                // The service might not be running, or it failed to provide characteristics.
                rootPage.NotifyUser("Restricted service. Can't read characteristics: " + ex.Message, NotifyType.ErrorMessage);
                return;
            }

            foreach (GattCharacteristic c in characteristics)
            {
                CharacteristicList.Items.Add(new ComboBoxItem { Content = DisplayHelpers.GetCharacteristicName(c), Tag = c });
            }
            CharacteristicList.Visibility = Visibility.Visible;
        }
        #endregion

        private void AddValueChangedHandler()
        {
            ValueChangedSubscribeToggle.Content = "Unsubscribe from value changes";
            if (registeredCharacteristic == null)
            {
                registeredCharacteristic = selectedCharacteristic;
                registeredCharacteristic.ValueChanged += Characteristic_ValueChanged;
            }
        }

        private void RemoveValueChangedHandler()
        {
            ValueChangedSubscribeToggle.Content = "Subscribe to value changes";
            if (registeredCharacteristic != null)
            {
                registeredCharacteristic.ValueChanged -= Characteristic_ValueChanged;
                registeredCharacteristic = null;
            }
        }

        private async void CharacteristicList_SelectionChanged(object sender, RoutedEventArgs e)
        {
            selectedCharacteristic = (GattCharacteristic)((ComboBoxItem)CharacteristicList.SelectedItem)?.Tag;
            if (selectedCharacteristic == null)
            {
                EnableCharacteristicPanels(GattCharacteristicProperties.None);
                rootPage.NotifyUser("No characteristic selected", NotifyType.ErrorMessage);
                return;
            }

            // Get all the child descriptors of a characteristics. Use the cache mode to specify uncached descriptors only
            // and the new Async functions to get the descriptors of unpaired devices as well.
            GattDescriptorsResult result = await selectedCharacteristic.GetDescriptorsAsync(BluetoothCacheMode.Uncached);
            if (result.Status != GattCommunicationStatus.Success)
            {
                rootPage.NotifyUser($"Descriptor read failure: {Utilities.FormatGattCommunicationStatus(result.Status, result.ProtocolError)}", NotifyType.ErrorMessage);
            }

            // Enable/disable operations based on the GattCharacteristicProperties.
            EnableCharacteristicPanels(selectedCharacteristic.CharacteristicProperties);
        }

        private void SetVisibility(UIElement element, bool visible)
        {
            element.Visibility = visible ? Visibility.Visible : Visibility.Collapsed;
        }

        private void EnableCharacteristicPanels(GattCharacteristicProperties properties)
        {
            // BT_Code: Hide the controls which do not apply to this characteristic.
            SetVisibility(CharacteristicReadButton, properties.HasFlag(GattCharacteristicProperties.Read));

            SetVisibility(CharacteristicWritePanel,
                properties.HasFlag(GattCharacteristicProperties.Write) ||
                properties.HasFlag(GattCharacteristicProperties.WriteWithoutResponse));
            CharacteristicWriteValue.Text = "";

            SetVisibility(ValueChangedSubscribeToggle, properties.HasFlag(GattCharacteristicProperties.Indicate) ||
                                                       properties.HasFlag(GattCharacteristicProperties.Notify));
            ValueChangedSubscribeToggle.IsEnabled =
                (registeredCharacteristic == null) ||
                (registeredCharacteristic == selectedCharacteristic);
        }

        private async void CharacteristicReadButton_Click(object sender, RoutedEventArgs e)
        {
            // Capture the characteristic we are reading from, in case the use changes the selection during the await.
            GattCharacteristic characteristic = selectedCharacteristic;

            // BT_Code: Read the actual value from the device by using Uncached.
            try
            {
                GattReadResult result = await selectedCharacteristic.ReadValueAsync(BluetoothCacheMode.Uncached);
                if (result.Status == GattCommunicationStatus.Success)
                {
                    string formattedResult = FormatValueByPresentation(characteristic, result.Value);
                    rootPage.NotifyUser($"Read result: {formattedResult}", NotifyType.StatusMessage);
                }
                else
                {
                    // This can happen when a device reports that it support reading, but it actually doesn't.
                    rootPage.NotifyUser($"Read failed: {Utilities.FormatGattCommunicationStatus(result.Status, result.ProtocolError)}", NotifyType.ErrorMessage);
                }
            }
            catch (ObjectDisposedException)
            {
                // Server is no longer available.
                rootPage.NotifyUser("Read failed: Service is no longer available.", NotifyType.ErrorMessage);
            }
        }

        private async void CharacteristicWriteButton_Click(object sender, RoutedEventArgs e)
        {
            if (!String.IsNullOrEmpty(CharacteristicWriteValue.Text))
            {
                var writeBuffer = CryptographicBuffer.ConvertStringToBinary(CharacteristicWriteValue.Text,
                    BinaryStringEncoding.Utf8);

                // WriteBufferToSelectedCharacteristicAsync will display an error message on failure
                // so we don't have to.
                await WriteBufferToSelectedCharacteristicAsync(writeBuffer);
            }
            else
            {
                rootPage.NotifyUser("No data to write to device", NotifyType.ErrorMessage);
            }
        }

        private async void CharacteristicWriteButtonInt_Click(object sender, RoutedEventArgs e)
        {
            if (Int32.TryParse(CharacteristicWriteValue.Text, out int writeValue))
            {
                // WriteBufferToSelectedCharacteristicAsync will display an error message on failure
                // so we don't have to.
                await WriteBufferToSelectedCharacteristicAsync(BufferHelpers.BufferFromInt32(writeValue));
            }
            else
            {
                rootPage.NotifyUser("Data to write has to be an int32", NotifyType.ErrorMessage);
            }
        }

        private async Task<bool> WriteBufferToSelectedCharacteristicAsync(IBuffer buffer)
        {
            // BT_Code: Writes the value from the buffer to the characteristic.
            try
            {
                GattWriteResult result = await selectedCharacteristic.WriteValueWithResultAsync(buffer);

                if (result.Status == GattCommunicationStatus.Success)
                {
                    rootPage.NotifyUser("Successfully wrote value to device", NotifyType.StatusMessage);
                    return true;
                }
                else
                {
                    // This can happen, for example, if a device reports that it supports writing, but it actually doesn't.
                    rootPage.NotifyUser($"Write failed: {Utilities.FormatGattCommunicationStatus(result.Status, result.ProtocolError)}", NotifyType.ErrorMessage);
                    return false;
                }
            }
            catch (ObjectDisposedException)
            {
                // Server is no longer available.
                rootPage.NotifyUser("Write failed: Service is no longer available.", NotifyType.ErrorMessage);
                return false;
            }
        }

        private async void ValueChangedSubscribeToggle_Click(object sender, RoutedEventArgs e)
        {
            string operation = null;
            // initialize status
            var cccdValue = GattClientCharacteristicConfigurationDescriptorValue.None;
            if (registeredCharacteristic != null)
            {
                // Unsubscribe by specifying "None"
                operation = "Unsubscribe";
                cccdValue = GattClientCharacteristicConfigurationDescriptorValue.None;
            }
            else if (selectedCharacteristic.CharacteristicProperties.HasFlag(GattCharacteristicProperties.Indicate))
            {
                // Subscribe with "indicate"
                operation = "Subscribe";
                cccdValue = GattClientCharacteristicConfigurationDescriptorValue.Indicate;
            }
            else if (selectedCharacteristic.CharacteristicProperties.HasFlag(GattCharacteristicProperties.Notify))
            {
                // Subscribe with "notify"
                operation = "Subscribe";
                cccdValue = GattClientCharacteristicConfigurationDescriptorValue.Notify;
            }
            else
            {
                // Unreachable because the button is disabled if it cannot indicate or notify.
            }

            // BT_Code: Must write the CCCD in order for server to send indications.
            // We receive them in the ValueChanged event handler.
            try
            {
                GattWriteResult result = await selectedCharacteristic.WriteClientCharacteristicConfigurationDescriptorWithResultAsync(cccdValue);

                if (result.Status == GattCommunicationStatus.Success)
                {
                    rootPage.NotifyUser($"{operation} succeeded", NotifyType.StatusMessage);
                    if (cccdValue != GattClientCharacteristicConfigurationDescriptorValue.None)
                    {
                        AddValueChangedHandler();
                    }
                    else
                    {
                        RemoveValueChangedHandler();
                    }
                }
                else
                {
                    // This can happen when a device reports that it supports indicate, but it actually doesn't.
                    rootPage.NotifyUser($"{operation} failed: {Utilities.FormatGattCommunicationStatus(result.Status, result.ProtocolError)}", NotifyType.ErrorMessage);
                }
            }
            catch (ObjectDisposedException)
            {
                // Service is no longer available.
                rootPage.NotifyUser($"{operation} failed: Service is no longer available.", NotifyType.ErrorMessage);
            }
        }

        private void Characteristic_ValueChanged(GattCharacteristic sender, GattValueChangedEventArgs args)
        {
            // BT_Code: An Indicate or Notify reported that the value has changed.
            // Display the new value with a timestamp.
            string newValue = FormatValueByPresentation(sender, args.CharacteristicValue);
            string message = $"Value of \"{DisplayHelpers.GetCharacteristicName(sender)}\" at {DateTime.Now:hh:mm:ss.FFF}: {newValue}";
            DispatcherQueue.TryEnqueue(() => CharacteristicLatestValue.Text = message);
        }

        private string FormatValueByPresentation(GattCharacteristic characteristic, IBuffer buffer)
        {
            // BT_Code: Choose a presentation format.
            GattPresentationFormat presentationFormat = null;
            if (characteristic.PresentationFormats.Count == 1)
            {
                // Get the presentation format since there's only one way of presenting it
                presentationFormat = characteristic.PresentationFormats[0];
            }
            else if (characteristic.PresentationFormats.Count > 1)
            {
                // It's difficult to figure out how to split up a characteristic and encode its different parts properly.
                // This sample doesn't try. It just encodes the whole thing to a string to make it easy to print out.
            }

            // BT_Code: For the purpose of this sample, this function converts only UInt32 and
            // UTF-8 buffers to readable text. It can be extended to support other formats if your app needs them.
            byte[] data;
            CryptographicBuffer.CopyToByteArray(buffer, out data);
            if (presentationFormat != null)
            {
                if (presentationFormat.FormatType == GattPresentationFormatTypes.UInt32 && data.Length >= 4)
                {
                    return BitConverter.ToInt32(data, 0).ToString();
                }
                else if (presentationFormat.FormatType == GattPresentationFormatTypes.Utf8)
                {
                    try
                    {
                        return Encoding.UTF8.GetString(data);
                    }
                    catch (ArgumentException)
                    {
                        return "(error: Invalid UTF-8 string)";
                    }
                }
                else
                {
                    // Add support for other format types as needed.
                    return "Unsupported format: " + CryptographicBuffer.EncodeToHexString(buffer);
                }
            }
            else if (data.Length == 0)
            {
                return "<empty data>";
            }
            // We don't know what format to use. Let's try some well-known profiles.
            else if (characteristic.Uuid.Equals(GattCharacteristicUuids.HeartRateMeasurement))
            {
                try
                {
                    return "Heart Rate: " + ParseHeartRateValue(data).ToString();
                }
                catch (ArgumentException)
                {
                    return "Heart Rate: (unable to parse)";
                }
            }
            else if (characteristic.Uuid.Equals(GattCharacteristicUuids.BatteryLevel))
            {
                // battery level is encoded as a percentage value in the first byte according to
                // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.battery_level.xml
                return "Battery Level: " + data[0].ToString() + "%";
            }
            // This is our custom calc service Result UUID. Format it like an Int
            else if (characteristic.Uuid.Equals(Constants.ResultCharacteristicUuid) ||
                characteristic.Uuid.Equals(Constants.BackgroundResultUuid))
            {
                return BitConverter.ToInt32(data, 0).ToString();
            }
            else
            {
                // Okay, so maybe UTF-8?
                try
                {
                    return "Unknown format: " + Encoding.UTF8.GetString(data);
                }
                catch (Exception)
                {
                    // Nope, not even UTF-8. Just show hex.
                    return "Unknown format: " + CryptographicBuffer.EncodeToHexString(buffer);
                }
            }
        }

        /// <summary>
        /// Process the raw data received from the device into application usable data,
        /// according the the Bluetooth Heart Rate Profile.
        /// https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.heart_rate_measurement.xml&u=org.bluetooth.characteristic.heart_rate_measurement.xml
        /// This function throws an exception if the data cannot be parsed.
        /// </summary>
        /// <param name="data">Raw data received from the heart rate monitor.</param>
        /// <returns>The heart rate measurement value.</returns>
        private static ushort ParseHeartRateValue(byte[] data)
        {
            // Heart Rate profile defined flag values
            const byte heartRateValueFormat16 = 0x01;

            byte flags = data[0];
            bool isHeartRateValueSizeLong = ((flags & heartRateValueFormat16) != 0);

            if (isHeartRateValueSizeLong)
            {
                return BitConverter.ToUInt16(data, 1);
            }
            else
            {
                return data[1];
            }
        }
    }
}


