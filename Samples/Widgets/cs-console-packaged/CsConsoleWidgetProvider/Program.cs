// Copyright (C) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.Windows.Widgets.Providers;
using System;
using WidgetHelper;

namespace CsConsoleWidgetProvider
{
    /// <summary>
    /// Main provider entrypoint.
    /// </summary>
    public static class Program
    {
        [MTAThread]
        static void Main(string[] args)
        {
            Console.WriteLine("CsConsoleWidgetProvider Starting...");
            if (args.Length > 0 && args[0] == "-RegisterProcessAsComServer")
            {
                WinRT.ComWrappersSupport.InitializeComWrappers();
                using (var manager = RegistrationManager<WidgetProvider>.RegisterProvider())
                {
                    Console.WriteLine("Widget Provider registered.");

                    var existingWidgets = WidgetManager.GetDefault().GetWidgetIds();
                    if (existingWidgets != null)
                    {
                        Console.WriteLine($"There are {existingWidgets.Length} Widgets currently outstanding:");
                        foreach (var widgetId in existingWidgets)
                        {
                            Console.WriteLine($"  {widgetId}");
                        }
                    }

                    Console.WriteLine("Press ENTER to exit.");
                    Console.ReadLine();
                }
            }
            else
            {
                Console.WriteLine("Not being launched to service Widget Provider... exiting.");
            }
        }
    }
}
