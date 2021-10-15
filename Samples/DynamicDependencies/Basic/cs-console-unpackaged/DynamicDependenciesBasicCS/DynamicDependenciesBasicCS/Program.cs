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
            //Initialize the Bootstrapper to load the Windows App SDK framework package
            Bootstrap.Initialize(0x00010000, "preview2");

            // Call a simple Windows App SDK api, and output the result
            // Off = 0, On = 1, Dimmed = 2,
            DisplayStatus dispStatus = PowerManager.DisplayStatus;

            Console.WriteLine("Hello World!");
            Console.WriteLine($"DisplayStatus: {dispStatus}");

            //Release the Dynamic Dependencies Lifetime Manager
            Bootstrap.Shutdown();
        }
    }
}
