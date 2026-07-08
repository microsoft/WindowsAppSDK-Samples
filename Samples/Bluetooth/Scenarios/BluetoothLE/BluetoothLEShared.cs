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
using System.Runtime.InteropServices;
using Windows.Devices.Bluetooth.GenericAttributeProfile;
using Windows.Foundation.Metadata;
using Windows.Security.Cryptography;
using Windows.Storage.Streams;

namespace SDKTemplate
{
    public static class BluetoothLEShared
    {
        public static string SelectedBleDeviceId;
        public static string SelectedBleDeviceName = "No device selected";
    }

    static class BufferHelpers
    {
        public static int? Int32FromBuffer(IBuffer buffer)
        {
            CryptographicBuffer.CopyToByteArray(buffer, out byte[] data);
            if (data.Length != sizeof(int))
            {
                return null;
            }

            return BitConverter.ToInt32(data, 0);
        }

        public static IBuffer BufferFromInt32(int value)
        {
            return CryptographicBuffer.CreateFromByteArray(BitConverter.GetBytes(value));
        }
    }
}

