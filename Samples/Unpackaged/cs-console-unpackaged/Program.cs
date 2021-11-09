// Copyright(c) Microsoft Corporation.
// Licensed under the MIT License.
using System;
using Microsoft.Windows.ApplicationModel.DynamicDependency;
using Microsoft.Windows.System.Power;

namespace DynamicDependenciesBasicCS
{
    class Program
    {
        static void Main(string[] args)
        {
            //For explicit initialization, call Bootstrap.Initialize() to load the Windows App SDK framework package
            //Remove WindowsPackageType=None in project file
            // Bootstrap.Initialize(0x00010000, "preview3");

            // Call a simple Windows App SDK API, and output the result
            DisplayStatus dispStatus = PowerManager.DisplayStatus;

            Console.WriteLine("Hello World!");
            Console.WriteLine($"DisplayStatus: {dispStatus}");

            //When performing explicit initialization, release the Dynamic Dependencies Lifetime Manager
            // Bootstrap.Shutdown();
        }
    }
}
