// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

using Microsoft.ApplicationModel.Resources;

namespace ClassLibrary
{
    public class Class
    {
        public string GetClassLibrarySampleString()
        {
            var resourceManager = new ResourceManager();
            return resourceManager.MainResourceMap.GetValue("ClassLibrary/ClassLibraryResources/ClassLibrarySampleString").ValueAsString;
        }
    }
}
