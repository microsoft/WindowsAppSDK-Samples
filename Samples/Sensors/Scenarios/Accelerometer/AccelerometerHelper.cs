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
using Microsoft.UI.Xaml.Controls;

namespace SDKTemplate
{
    // Holds state shared between the Accelerometer scenarios. In the UWP sample these lived on
    // the per-sample MainPage; in this consolidated sample MainPage is the shared shell, so the
    // accelerometer-specific state lives here instead.
    static class AccelerometerHelper
    {
        public static AccelerometerReadingType ReadingType { get; set; } = AccelerometerReadingType.Standard;

        public static void SetReadingText(TextBlock textBlock, AccelerometerReading reading)
        {
            textBlock.Text = string.Format("X: {0,5:0.00}, Y: {1,5:0.00}, Z: {2,5:0.00}",
                reading.AccelerationX, reading.AccelerationY, reading.AccelerationZ);
        }
    }
}
