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
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario1_CameraManualControls : Page
    {

        private void UpdateIsoControlCapabilities()
        {
            var isoSpeedControl = _mediaCapture.VideoDeviceController.IsoSpeedControl;

            if (isoSpeedControl.Supported)
            {
                IsoButton.Tag = Visibility.Visible;
                IsoButton.IsEnabled = true;

                IsoAutoCheckBox.IsChecked = isoSpeedControl.Auto;

                // Unhook the event handler, so that changing properties on the slider won't trigger an API call
                IsoSlider.ValueChanged -= IsoSlider_ValueChanged;

                var value = isoSpeedControl.Value;
                IsoSlider.Minimum = isoSpeedControl.Min;
                IsoSlider.Maximum = isoSpeedControl.Max;
                IsoSlider.StepFrequency = isoSpeedControl.Step;
                IsoSlider.Value = value;

                IsoSlider.ValueChanged += IsoSlider_ValueChanged;
            }
            else
            {
                IsoButton.IsEnabled = false;
                IsoButton.Tag = Visibility.Visible;
            }
        }

        private async void IsoSlider_ValueChanged(object sender, Microsoft.UI.Xaml.Controls.Primitives.RangeBaseValueChangedEventArgs e)
        {
            if (_settingUpUi) return;

            var value = (sender as Slider).Value;

            await _mediaCapture.VideoDeviceController.IsoSpeedControl.SetValueAsync((uint)value);
        }

        private async void IsoAutoCheckBox_CheckedChanged(object sender, RoutedEventArgs e)
        {
            var autoIso = (sender as CheckBox).IsChecked == true;

            if (autoIso)
            {
                await _mediaCapture.VideoDeviceController.IsoSpeedControl.SetAutoAsync();
            }
            else
            {
                await _mediaCapture.VideoDeviceController.IsoSpeedControl.SetValueAsync((uint)IsoSlider.Value);
            }
        }

    }
}

