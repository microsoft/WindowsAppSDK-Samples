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
        Auto,    // Auto-select based on device type
        FP32,    // 32-bit floating point
        INT8,    // 8-bit integer quantized
        QDQ      // Quantize-Dequantize format
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
        public string ModelPath { get; set; } = string.Empty;
        public string OutputPath { get; set; } = "SqueezeNet_ctx.onnx";
        public string ImagePath { get; set; } = string.Empty;
        public ModelVariant Variant { get; set; } = ModelVariant.Auto;
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

                    case "--fp32":
                        options.Variant = ModelVariant.FP32;
                        break;

                    case "--int8":
                        options.Variant = ModelVariant.INT8;
                        break;

                    case "--qdq":
                        options.Variant = ModelVariant.QDQ;
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

            // Auto-deduce model variant from execution provider if not explicitly set and no custom model specified
            if (options.Variant == ModelVariant.Auto && string.IsNullOrWhiteSpace(options.ModelPath))
            {
                if (options.EpPolicy.HasValue)
                {
                    options.Variant = options.EpPolicy.Value switch
                    {
                        ExecutionProviderDevicePolicy.PREFER_NPU or ExecutionProviderDevicePolicy.PREFER_CPU => ModelVariant.INT8,
                        ExecutionProviderDevicePolicy.PREFER_GPU or ExecutionProviderDevicePolicy.DEFAULT or _ => ModelVariant.FP32
                    };
                    Console.WriteLine($"Auto-selected {options.Variant} model variant for {options.EpPolicy.Value} execution");
                }
                else
                {
                    // Default to FP32 when using explicit EP name
                    options.Variant = ModelVariant.FP32;
                    Console.WriteLine("Auto-selected FP32 model variant for explicit execution provider");
                }
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
            Console.WriteLine("  --model <path>              Path to input ONNX model (optional, default: SqueezeNet)");
            Console.WriteLine("                              If not provided, you can specify model variant with:");
            Console.WriteLine("                                --fp32    FP32 model variant (32-bit floating point)");
            Console.WriteLine("                                --int8    INT8 model variant (8-bit integer quantized)");
            Console.WriteLine("                                --qdq     QDQ model variant (Quantize-Dequantize format)");
            Console.WriteLine("                              Auto-selection: INT8 for NPU/CPU, FP32 for GPU/DEFAULT");
            Console.WriteLine("  --compiled_output <path>    Path for compiled output model (default: SqueezeNet_ctx.onnx)");
            Console.WriteLine("  --image_path <path>         Path to the input image (default: sample kitten image)");
            Console.WriteLine("  --help, -h                  Display this help message");
            Console.WriteLine();
            Console.WriteLine("Exactly one of --ep_policy or --ep_name must be specified.");
            Console.WriteLine();
            Console.WriteLine("Available execution providers (name, vendor, device type):");
            ExecutionProviderManager.PrintExecutionProviderHelpTable();
        }
    }
}
