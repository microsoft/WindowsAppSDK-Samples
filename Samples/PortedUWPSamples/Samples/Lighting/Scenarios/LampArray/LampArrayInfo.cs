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

using Windows.Devices.Lights;

namespace SDKTemplate
{
    internal class LampArrayInfo
    {
        public LampArrayInfo(string id, string displayName, LampArray lampArray)
        {
            this.id = id;
            this.displayName = displayName;
            this.lampArray = lampArray;
        }

        public readonly string id;
        public readonly string displayName;
        public readonly LampArray lampArray;
    }
}