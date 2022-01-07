// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.Windows.ApplicationModel.Resources;

var manager = new ResourceManager("SelfContainedDeployment.pri");

Console.WriteLine(manager.MainResourceMap.GetValue("Resources/Description").ValueAsString);