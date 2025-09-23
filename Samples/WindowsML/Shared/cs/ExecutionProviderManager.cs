// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.ML.OnnxRuntime;
using Microsoft.Windows.AI.MachineLearning;

namespace WindowsML.Shared
{
    /// <summary>
    /// Execution Provider configuration and discovery
    /// </summary>
    public static class ExecutionProviderManager
    {
        /// <summary>
        /// Initialize execution providers with download control
        /// </summary>
        public static async Task InitializeProvidersAsync(bool allowDownload = false)
        {
            Console.WriteLine("Getting available providers...");
            var catalog = ExecutionProviderCatalog.GetDefault();
            var providers = catalog.FindAllProviders();

            if (providers is null || providers.Length == 0)
            {
                Console.WriteLine("No execution providers found in catalog.");
                return;
            }

            foreach (var provider in providers)
            {
                Console.WriteLine($"Provider: {provider.Name}");
                try
                {
                    var readyState = provider.ReadyState;
                    Console.WriteLine($"  Ready state: {readyState}");

                    // Only call EnsureReadyAsync if we allow downloads or if the provider is already ready
                    if (allowDownload || readyState != ExecutionProviderReadyState.NotPresent)
                    {
                        await provider.EnsureReadyAsync();
                    }

                    provider.TryRegister();
                }
                catch (Exception ex)
                {
                    Console.WriteLine($"  Failed to initialize provider {provider.Name}: {ex.Message}");
                    // Continue with other providers
                }
            }
        }
        /// <summary>
        /// Print execution provider device information
        /// </summary>
        public static void PrintDeviceInfo(Dictionary<string, List<OrtEpDevice>> epDeviceMap)
        {
            // Print table headers
            Console.WriteLine("Execution Provider Information:");
            Console.WriteLine("-------------------------------------------------------------");
            Console.WriteLine("{0,-32} {1,-16} {2,-12}", "Provider", "Vendor", "Device Type");
            Console.WriteLine("-------------------------------------------------------------");

            // Print each device with its provider name
            foreach (KeyValuePair<string, List<OrtEpDevice>> epGroup in epDeviceMap)
            {
                foreach (var device in epGroup.Value)
                {
                    Console.WriteLine("{0,-32} {1,-16} {2,-12}",
                        device.EpName,
                        device.EpVendor,
                        device.HardwareDevice.Type.ToString());
                }
            }

            Console.WriteLine("-------------------------------------------------------------");
        }

        /// <summary>
        /// Configure one explicitly selected execution provider (optional device type if provider exposes multiple).
        /// Returns true on success, false otherwise (prints guidance on failure).
        /// </summary>
        public static bool ConfigureSelectedExecutionProvider(SessionOptions sessionOptions,
                                                              OrtEnv environment,
                                                              string epName,
                                                              string? deviceType)
        {
            // Discover devices
            IReadOnlyList<OrtEpDevice> epDevices = environment.GetEpDevices();

            // Accumulate devices by EP name
            var epDeviceMap = new Dictionary<string, List<OrtEpDevice>>(StringComparer.OrdinalIgnoreCase);
            foreach (var d in epDevices)
            {
                if (!epDeviceMap.TryGetValue(d.EpName, out var list))
                {
                    list = new List<OrtEpDevice>();
                    epDeviceMap[d.EpName] = list;
                }
                list.Add(d);
            }

            if (string.IsNullOrWhiteSpace(epName))
            {
                Console.WriteLine("ERROR: --ep_name is required when not using --ep_policy.");
                PrintDeviceInfo(epDeviceMap);
                return false;
            }

            if (!epDeviceMap.TryGetValue(epName, out var devicesForEp))
            {
                Console.WriteLine($"ERROR: Execution provider '{epName}' not found among discovered devices.");
                PrintDeviceInfo(epDeviceMap);
                return false;
            }

            var selectedDevices = new List<OrtEpDevice>();

            if (!string.IsNullOrWhiteSpace(deviceType))
            {
                Console.WriteLine($"Searching for device type '{deviceType}' for provider '{epName}'.");
                var match = devicesForEp.FirstOrDefault(d => string.Equals(d.HardwareDevice.Type.ToString(), deviceType, StringComparison.OrdinalIgnoreCase));
                if (match == null)
                {
                    Console.WriteLine($"ERROR: Device type '{deviceType}' not found for provider '{epName}'.");
                    return false;
                }
                selectedDevices.Add(match);
                Console.WriteLine($"Using device type '{deviceType}' for provider '{epName}'.");
            }
            else
            {
                // No device type specified: use all devices for this EP
                selectedDevices.AddRange(devicesForEp);
                Console.WriteLine($"No device type specified; using the following device(s) for provider '{epName}':");
                foreach (var d in selectedDevices)
                {
                    Console.WriteLine($"  {d.HardwareDevice.Type}");
                }
            }

            try
            {
                var epOptions = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);
                sessionOptions.AppendExecutionProvider(environment, selectedDevices, epOptions);
                Console.WriteLine($"Successfully added {epName} execution provider");
                return true;
            }
            catch (Exception ex)
            {
                Console.WriteLine($"ERROR: Failed to append execution provider '{epName}': {ex.Message}");
                return false;
            }
        }

        public static async Task PrintExecutionProviderHelpTable()
        {
            try
            {
                // Call InitializeProvidersAsync first to ensure any providers from the catalog are registered and show up in the list
                await InitializeProvidersAsync(allowDownload: false);

                EnvironmentCreationOptions envOptions = new() {
                };

                var env = OrtEnv.CreateInstanceWithOptions(ref envOptions);
                var devices = env.GetEpDevices();
                if (devices.Count == 0)
                {
                    Console.WriteLine("(No execution provider devices discovered)");
                    return;
                }
                var epDeviceMap = new Dictionary<string, List<OrtEpDevice>>(StringComparer.OrdinalIgnoreCase);
                foreach (var d in devices)
                {
                    if (!epDeviceMap.TryGetValue(d.EpName, out var list))
                    {
                        list = new List<OrtEpDevice>();
                        epDeviceMap[d.EpName] = list;
                    }
                    list.Add(d);
                }
                PrintDeviceInfo(epDeviceMap);
            }
            catch
            {
                Console.WriteLine("(Execution provider enumeration unavailable in help context)");
            }
        }
    }
}
