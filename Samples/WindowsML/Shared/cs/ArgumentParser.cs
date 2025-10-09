// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.

using Microsoft.ML.OnnxRuntime;
using System.IO;
using System.Reflection;

namespace WindowsML.Shared
{
    /// <summary>
    /// Model variant enumeration
    /// </summary>
    public enum ModelVariant
    {
        Default,    // Default quantized model (for NPU/CPU)
        FP32        // FP32 non-quantized model (for GPU)
    }

    /// <summary>
    /// Command-line argument parsing for WindowsML samples
    /// </summary>
    public class Options
    {
        public ExecutionProviderDevicePolicy? EpPolicy { get; set; } = null; // Policy-based (mutually exclusive with EpName)
        public string? EpName { get; set; } = null; // Explicit EP name (mutually exclusive with EpPolicy)
        public string? DeviceType { get; set; } = null; // Optional value to declare CPU | GPU | NPU
        public bool Compile { get; set; } = false;
        public bool Download { get; set; } = false;
        public bool UseModelCatalog { get; set; } = false;
        public string ModelPath { get; set; } = string.Empty;
        public string OutputPath { get; set; } = "SqueezeNet_ctx.onnx";
        public string ImagePath { get; set; } = string.Empty;
        public ModelVariant Variant { get; set; } = ModelVariant.Default;
    }

    public static class ArgumentParser
    {
        /// <summary>
        /// Parse command-line arguments
        /// </summary>
        public static Options ParseOptions(string[] args)
        {
            Options options = new();

            for (int i = 0; i < args.Length; i++)
            {
                switch (args[i])
                {
                    case "--ep_policy":
                        if (i + 1 < args.Length)
                        {
                            string policyStr = args[++i];
                            switch (policyStr.ToUpper())
                            {
                                case "NPU":
                                    options.EpPolicy = ExecutionProviderDevicePolicy.PREFER_NPU;
                                    break;
                                case "CPU":
                                    options.EpPolicy = ExecutionProviderDevicePolicy.PREFER_CPU;
                                    break;
                                case "GPU":
                                    options.EpPolicy = ExecutionProviderDevicePolicy.PREFER_GPU;
                                    break;
                                case "DEFAULT":
                                    options.EpPolicy = ExecutionProviderDevicePolicy.DEFAULT;
                                    break;
                                case "DISABLE":
                                    options.EpPolicy = null;
                                    break;
                                default:
                                    Console.WriteLine($"Unknown EP policy: {policyStr}, using default (DISABLE)");
                                    options.EpPolicy = null;
                                    break;
                            }
                        }
                        break;

                    case "--ep_name":
                        if (i + 1 < args.Length)
                        {
                            options.EpName = args[++i];
                        }
                        break;

                    case "--device_type":
                        if (i + 1 < args.Length)
                        {
                            options.DeviceType = args[++i].ToUpperInvariant();
                        }
                        break;

                    case "--compile":
                        options.Compile = true;
                        break;

                    case "--download":
                        options.Download = true;
                        break;
                        
                    case "--use_model_catalog":
                        options.UseModelCatalog = true;
                        break;

                    case "--model":
                        if (i + 1 < args.Length)
                        {
                            options.ModelPath = args[++i];
                        }
                        break;

                    case "--compiled_output":
                        if (i + 1 < args.Length)
                        {
                            options.OutputPath = args[++i];
                        }
                        break;

                    case "--image_path":
                        if (i + 1 < args.Length)
                        {
                            options.ImagePath = args[++i];
                        }
                        break;
                    case "--help":
                    case "-h":
                        PrintHelp();
                        throw new Exception("Help requested");
                    default:
                        PrintHelp();
                        throw new Exception($"Invalid argument: {args[i]}");
                }
            }

            // Mutual exclusivity validation
            if (options.EpPolicy.HasValue && !string.IsNullOrEmpty(options.EpName))
            {
                Console.WriteLine("ERROR: Specify only one of --ep_policy or --ep_name.");
                PrintHelp();
                throw new Exception("Mutually exclusive EP options");
            }

            if (!options.EpPolicy.HasValue && string.IsNullOrEmpty(options.EpName))
            {
                Console.WriteLine("ERROR: You must specify one of --ep_policy or --ep_name.");
                PrintHelp();
                throw new Exception("Missing EP selection");
            }

            if (!string.IsNullOrEmpty(options.DeviceType))
            {
                if (string.IsNullOrEmpty(options.EpName))
                {
                    Console.WriteLine("ERROR: --device_type requires --ep_name to be specified.");
                    PrintHelp();
                    throw new Exception("Missing ep_name for device_type");
                }

                if (options.DeviceType != "CPU" && options.DeviceType != "GPU" && options.DeviceType != "NPU")
                {
                    Console.WriteLine("ERROR: Invalid --device_type value. Valid values: CPU GPU NPU.");
                    PrintHelp();
                    throw new Exception("Invalid device_type value");
                }
            }

            // Validate mandatory fields
            if (string.IsNullOrWhiteSpace(options.ImagePath))
            {
                string executableFolder = Path.GetDirectoryName(Assembly.GetEntryAssembly()!.Location)!;
                options.ImagePath = Path.Combine(executableFolder, "image.png");
            }

            return options;
        }

        /// <summary>
        /// Print help message
        /// </summary>
        public static void PrintHelp()
        {
            Console.WriteLine("Options:");
            Console.WriteLine("  --ep_policy <policy>        Set execution provider policy (NPU, CPU, GPU, DEFAULT, DISABLE)");
            Console.WriteLine("  --ep_name <name>            Explicit execution provider name (mutually exclusive with --ep_policy)");
            Console.WriteLine("  --device_type <type>        Optional hardware device type to use when EP supports multiple (e.g. CPU, GPU, NPU)");
            Console.WriteLine("  --compile                   Compile the model");
            Console.WriteLine("  --download                  Download required packages");
            Console.WriteLine("  --use_model_catalog         Use the model catalog for model discovery");
            Console.WriteLine("  --model <path>              Path to input ONNX model (default: SqueezeNet.onnx)");
            Console.WriteLine("  --compiled_output <path>    Path for compiled output model (default: SqueezeNet_ctx.onnx)");
            Console.WriteLine("  --image_path <path>         Path to the input image (default: sample kitten image)");
            Console.WriteLine("  --help, -h                  Display this help message");
            Console.WriteLine();
            Console.WriteLine("Exactly one of --ep_policy or --ep_name must be specified.");
            Console.WriteLine();
            Console.WriteLine("Available execution providers (name, vendor, device type):");
            ExecutionProviderManager
                .PrintExecutionProviderHelpTable()
                .GetAwaiter()
                .GetResult();
        }
    }
}
