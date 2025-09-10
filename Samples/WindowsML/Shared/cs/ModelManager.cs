// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.

using Microsoft.ML.OnnxRuntime;
using System.IO;
using System.Reflection;

namespace WindowsML.Shared
{
    /// <summary>
    /// Model management functionality
    /// </summary>
    public static class ModelManager
    {
        /// <summary>
        /// Create ONNX Runtime environment
        /// </summary>
        public static OrtEnv CreateEnvironment(string logId = "WindowsMLSample")
        {
            // Create a new instance of EnvironmentCreationOptions
            EnvironmentCreationOptions envOptions = new() {
                logId = logId,
                logLevel = OrtLoggingLevel.ORT_LOGGING_LEVEL_WARNING
            };

            // Pass the options by reference to CreateInstanceWithOptions
            return OrtEnv.CreateInstanceWithOptions(ref envOptions);
        }

        /// <summary>
        /// Initialize execution provider catalog
        /// </summary>
        public static async System.Threading.Tasks.Task InitializeExecutionProvidersAsync(bool allowDownload = false)
        {
            await ExecutionProviderManager.InitializeProvidersAsync(allowDownload);
        }

        /// <summary>
        /// Resolve model paths
        /// </summary>
        public static (string modelPath, string compiledModelPath, string labelsPath) ResolvePaths(Options options)
        {
            string executableFolder = Path.GetDirectoryName(Assembly.GetEntryAssembly()!.Location)!;

            string modelPath = options.ModelPath.Contains(Path.DirectorySeparatorChar) ?
                options.ModelPath : Path.Combine(executableFolder, options.ModelPath);

            string compiledModelPath = options.OutputPath.Contains(Path.DirectorySeparatorChar) ?
                options.OutputPath : Path.Combine(executableFolder, options.OutputPath);

            string labelsPath = Path.Combine(executableFolder, "SqueezeNet.Labels.txt");

            if (!File.Exists(labelsPath))
            {
                throw new FileNotFoundException($"Labels file not found: {labelsPath}");
            }

            return (modelPath, compiledModelPath, labelsPath);
        }

        /// <summary>
        /// Create session with execution provider configuration
        /// </summary>
        public static InferenceSession CreateSession(string modelPath, Options options, OrtEnv ortEnv)
        {
            Console.WriteLine("Creating session ...");
            SessionOptions sessionOptions = new();

            // The platform allows the caller to specify what EP should be preferred for executing the model graph. There
            // are fallbacks in place if the desired hardware is not present on the device. For example, CPU is always
            // available to guarantee the model can execute. In this example, the caller expresses preference for the NPU,
            // with a fallback to the CPU if portions of the graph cannot be executed by the available NPU EP.

            // Set EP selection policy from options
            if (options.EpPolicy.HasValue)
            {
                Console.WriteLine($"Using EP selection policy: {options.EpPolicy.Value}");
                sessionOptions.SetEpSelectionPolicy(options.EpPolicy.Value);
            }
            else
            {
                Console.WriteLine("Using explicit EP configuration");
                ExecutionProviderManager.ConfigureExecutionProviders(sessionOptions, ortEnv);
            }

            return new InferenceSession(modelPath, sessionOptions);
        }

        /// <summary>
        /// Compile model
        /// </summary>
        public static void CompileModel(SessionOptions sessionOptions, string modelPath, string compiledModelPath)
        {
            Console.WriteLine($"Compiling model from {modelPath}");
            Console.WriteLine($"Output path: {compiledModelPath}");

            // Create compilation options from session options
            OrtModelCompilationOptions compileOptions = new(sessionOptions);

            try
            {
                // Set input and output model paths
                compileOptions.SetInputModelPath(modelPath);
                compileOptions.SetOutputModelPath(compiledModelPath);

                Console.WriteLine("Starting compile, this may take a few moments...");
                DateTime start = DateTime.Now;

                // Compile the model
                compileOptions.CompileModel();

                TimeSpan duration = DateTime.Now - start;
                Console.WriteLine($"Model compiled successfully in {duration.TotalMilliseconds} ms");
            }
            catch
            {
                Console.Error.WriteLine($"Failed to compile model, continuing ...");
            }
        }

        /// <summary>
        /// Determine actual model path to use (with compilation logic)
        /// </summary>
        public static string ResolveActualModelPath(Options options, string modelPath, string compiledModelPath, OrtEnv ortEnv)
        {
            string actualModelPath;
            bool isCompiledModelAvailable = File.Exists(compiledModelPath);

            if (isCompiledModelAvailable)
            {
                Console.WriteLine($"Using existing compiled model: {compiledModelPath}");
                actualModelPath = compiledModelPath;
            }
            else if (options.Compile)
            {
                Console.WriteLine("No compiled model found, attempting to create compiled model");

                try
                {
                    SessionOptions tempSessionOptions = new();
                    if (options.EpPolicy.HasValue)
                    {
                        tempSessionOptions.SetEpSelectionPolicy(options.EpPolicy.Value);
                    }
                    else
                    {
                        ExecutionProviderManager.ConfigureExecutionProviders(tempSessionOptions, ortEnv);
                    }

                    CompileModel(tempSessionOptions, modelPath, compiledModelPath);

                    if (File.Exists(compiledModelPath))
                    {
                        Console.WriteLine($"Compiled model created successfully at {compiledModelPath}");
                        actualModelPath = compiledModelPath;
                    }
                    else
                    {
                        Console.WriteLine($"Falling back to original model: {modelPath}");
                        actualModelPath = modelPath;
                    }
                }
                catch (Exception ex)
                {
                    Console.WriteLine($"Compilation failed: {ex.Message}");
                    Console.WriteLine($"Falling back to original model: {modelPath}");
                    actualModelPath = modelPath;
                }
            }
            else
            {
                Console.WriteLine($"Using original model: {modelPath}");
                actualModelPath = modelPath;
            }

            return actualModelPath;
        }
    }
}
