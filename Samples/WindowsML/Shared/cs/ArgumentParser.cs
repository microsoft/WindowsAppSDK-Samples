// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.

using Microsoft.ML.OnnxRuntime;
using System.IO;
using System.Reflection;

namespace WindowsML.Shared
{
    /// <summary>
    /// Command-line argument parsing for WindowsML samples
    /// </summary>
    public class Options
    {
        public ExecutionProviderDevicePolicy? EpPolicy { get; set; } = null; // Default: null (DISABLE)
        public bool Compile { get; set; } = false;
        public bool Download { get; set; } = false;
        public string ModelPath { get; set; } = "SqueezeNet.onnx"; // Default: SqueezeNet.onnx
        public string OutputPath { get; set; } = "SqueezeNet_ctx.onnx"; // Default: SqueezeNet_ctx.onnx
        public string ImagePath { get; set; } = string.Empty; // Mandatory field
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
                    case "--help":
                    case "-h":
                    default:
                        PrintHelp();
                        throw new Exception($"Invalid arguments");
                }
            }

            // Validate mandatory fields
            if (string.IsNullOrWhiteSpace(options.ImagePath))
            {
                string executableFolder = Path.GetDirectoryName(Assembly.GetEntryAssembly()!.Location)!;
                options.ImagePath = Path.Combine(executableFolder, "image.jpg");
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
            Console.WriteLine("  --compile                   Compile the model");
            Console.WriteLine("  --download                  Download required packages");
            Console.WriteLine("  --model <path>              Path to input ONNX model (default: SqueezeNet.onnx)");
            Console.WriteLine("  --compiled_output <path>    Path for compiled output model (default: SqueezeNet_ctx.onnx)");
            Console.WriteLine("  --image_path <path>         Path to the input image (default: image.jpg in the executable directory)");
            Console.WriteLine("  --help, -h                  Display this help message");
        }
    }
}
