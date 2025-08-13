// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.

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
                List<OrtEpDevice> devices = epGroup.Value;

                foreach (OrtEpDevice device in devices)
                {
                    string epName = device.EpName;
                    string vendor = device.EpVendor;
                    string deviceType = device.HardwareDevice.Type.ToString();

                    Console.WriteLine("{0,-32} {1,-16} {2,-12}", epName, vendor, deviceType);
                }
            }

            Console.WriteLine("-------------------------------------------------------------");
        }

        /// <summary>
        /// Configure execution providers
        /// </summary>
        public static void ConfigureExecutionProviders(SessionOptions sessionOptions, OrtEnv environment)
        {
            // Get all available EP devices from the environment
            IReadOnlyList<OrtEpDevice> epDevices = environment.GetEpDevices();

            Console.WriteLine($"Discovered {epDevices.Count} execution provider device(s)");

            // Accumulate devices by ep_name
            Dictionary<string, List<OrtEpDevice>> epDeviceMap = new(StringComparer.OrdinalIgnoreCase);

            // Group devices by EP name
            foreach (OrtEpDevice device in epDevices)
            {
                string epName = device.EpName;

                if (!epDeviceMap.ContainsKey(epName))
                {
                    epDeviceMap[epName] = [];
                }

                epDeviceMap[epName].Add(device);
            }

            // Print the execution provider information
            PrintDeviceInfo(epDeviceMap);

            // Configure execution providers
            foreach (KeyValuePair<string, List<OrtEpDevice>> epGroup in epDeviceMap)
            {
                string epName = epGroup.Key;
                List<OrtEpDevice> devices = epGroup.Value;

                // Configure EP with all its devices
                Dictionary<string, string> epOptions = new(StringComparer.OrdinalIgnoreCase);

                switch (epName)
                {
                    case "VitisAIExecutionProvider":
                        sessionOptions.AppendExecutionProvider(environment, devices, epOptions);
                        Console.WriteLine($"Successfully added {epName} EP");
                        break;

                    case "OpenVINOExecutionProvider":
                        // Append the first device
                        sessionOptions.AppendExecutionProvider(environment, [devices[0]], epOptions);
                        Console.WriteLine($"Successfully added {epName} EP (first device only)");
                        break;

                    case "QNNExecutionProvider":
                        // Configure performance mode for QNN EP
                        epOptions["htp_performance_mode"] = "high_performance";
                        sessionOptions.AppendExecutionProvider(environment, devices, epOptions);
                        Console.WriteLine($"Successfully added {epName} EP");
                        break;

                    case "NvTensorRTRTXExecutionProvider":
                        // Configure performance mode for TensorRT RTX EP
                        sessionOptions.AppendExecutionProvider(environment, devices, epOptions);
                        Console.WriteLine($"Successfully added {epName} EP");
                        break;

                    default:
                        break;
                }
            }
        }
    }
}
