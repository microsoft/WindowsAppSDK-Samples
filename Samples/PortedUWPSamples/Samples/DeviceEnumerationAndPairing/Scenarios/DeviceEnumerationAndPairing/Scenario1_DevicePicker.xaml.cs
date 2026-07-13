// Copyright (c) Microsoft. All rights reserved.

using System;
using System.Collections.ObjectModel;
using Windows.Devices.Enumeration;
using Windows.Foundation;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario1_DevicePicker : Page
    {
        private MainPage rootPage = MainPage.Current;

        private ObservableCollection<DeviceInformationDisplay> resultCollection = new ObservableCollection<DeviceInformationDisplay>();

        public Scenario1_DevicePicker()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            resultsListView.ItemsSource = resultCollection;
            selectorComboBox.ItemsSource = DeviceSelectorChoices.DevicePickerSelectors;
            selectorComboBox.SelectedIndex = 0;
        }

        private void PickSingleDeviceButton_Click(object sender, RoutedEventArgs e)
        {
            ShowDevicePicker(
                true // pickSingle
                );
        }

        private void ShowDevicePickerButton_Click(object sender, RoutedEventArgs e)
        {
            ShowDevicePicker(
                false // pickSingle
                );
        }

        private async void ShowDevicePicker(bool pickSingle)
        {
            showDevicePickerButton.IsEnabled = false;
            resultCollection.Clear();

            var devicePicker = new DevicePicker();
            WinRT.Interop.InitializeWithWindow.Initialize(devicePicker, WinRT.Interop.WindowNative.GetWindowHandle(App.MainWindow));

            // First get the device selector chosen by the UI.
            DeviceSelectorInfo deviceSelectorInfo = (DeviceSelectorInfo)selectorComboBox.SelectedItem;

            if (null == deviceSelectorInfo.Selector)
            {
                // If the a pre-canned device class selector was chosen, call the DeviceClass overload
                devicePicker.Filter.SupportedDeviceClasses.Add(deviceSelectorInfo.DeviceClassSelector);
            }
            else
            {
                // Use AQS string selector from dynamic call to a device api's GetDeviceSelector call
                devicePicker.Filter.SupportedDeviceSelectors.Add(deviceSelectorInfo.Selector);
            }

            rootPage.NotifyUser("Showing Device Picker", NotifyType.StatusMessage);

            // Calculate the position to show the picker (right below the buttons)
            GeneralTransform ge = pickSingleDeviceButton.TransformToVisual(null);
            Point point = ge.TransformPoint(new Point());
            Rect rect = new Rect(point, new Point(point.X + pickSingleDeviceButton.ActualWidth, point.Y + pickSingleDeviceButton.ActualHeight));

            if (pickSingle)
            {
                DeviceInformation di = await devicePicker.PickSingleDeviceAsync(rect);
                if (null != di)
                {
                    resultCollection.Add(new DeviceInformationDisplay(di));
                }
                showDevicePickerButton.IsEnabled = true;
            }
            else
            {
                devicePicker.DevicePickerDismissed += new TypedEventHandler<DevicePicker, object>(
                (picker, obj) =>
                {
                    // Must access UI elements from the UI thread.
                    this.DispatcherQueue.TryEnqueue( () =>
                    {
                        showDevicePickerButton.IsEnabled = true;
                        rootPage.NotifyUser("Hiding Device Picker", NotifyType.StatusMessage);
                    });
                });

                devicePicker.DeviceSelected += new TypedEventHandler<DevicePicker, DeviceSelectedEventArgs>(
                (picker, args) =>
                {
                    // Since we have the collection databound to a UI element, we need to update the collection on the UI thread.
                    this.DispatcherQueue.TryEnqueue( () =>
                    {
                        resultCollection.Clear();
                        resultCollection.Add(new DeviceInformationDisplay(args.SelectedDevice));
                    });
                });

                // Show the picker
                devicePicker.Show(rect);
            }
        }
    }
}
