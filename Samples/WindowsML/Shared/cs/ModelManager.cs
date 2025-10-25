// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.

using Microsoft.ML.OnnxRuntime;
using System.IO;
using System.Reflection;
using Microsoft.Windows.AI.MachineLearning;

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
        /// Determine the best model variant based on execution provider configuration
        /// </summary>
        public static ModelVariant DetermineModelVariant(Options options, OrtEnv ortEnv)
        {
            // If user explicitly set a custom model path, variant doesn't matter
            if (!string.IsNullOrWhiteSpace(options.ModelPath))
            {
                return options.Variant; // Use whatever was set (default is Default)
            }

            // For EP policy, we can determine based on the policy
            if (options.EpPolicy.HasValue)
            {
                var variant = options.EpPolicy.Value switch
                {
                    ExecutionProviderDevicePolicy.PREFER_GPU => ModelVariant.FP32,
                    ExecutionProviderDevicePolicy.PREFER_NPU or ExecutionProviderDevicePolicy.PREFER_CPU or ExecutionProviderDevicePolicy.DEFAULT or _ => ModelVariant.Default
                };
                Console.WriteLine($"Auto-selected {variant} model variant for {options.EpPolicy.Value} execution");
                return variant;
            }

            // For explicit EP name, we need to check what device types are available
            if (!string.IsNullOrEmpty(options.EpName))
            {
                try
                {
                    var devices = ortEnv.GetEpDevices();
                    var epDevices = devices.Where(d => d.EpName.Equals(options.EpName, StringComparison.OrdinalIgnoreCase)).ToList();
                    
                    // If user specified a device type, use that
                    if (!string.IsNullOrEmpty(options.DeviceType))
                    {
                        var variant = options.DeviceType.ToUpperInvariant() == "GPU" ? ModelVariant.FP32 : ModelVariant.Default;
                        Console.WriteLine($"Auto-selected {variant} model variant for {options.EpName} with device type {options.DeviceType}");
                        return variant;
                    }
                    
                    // Otherwise, check if any of the available devices for this EP are GPU
                    bool hasGpu = epDevices.Any(d => d.HardwareDevice.Type.ToString().Equals("GPU", StringComparison.OrdinalIgnoreCase));
                    if (hasGpu)
                    {
                        Console.WriteLine($"Auto-selected FP32 model variant for {options.EpName} (GPU device available)");
                        return ModelVariant.FP32;
                    }
                }
                catch (Exception ex)
                {
                    Console.WriteLine($"Warning: Could not determine device type for {options.EpName}: {ex.Message}");
                }
            }

            // Default fallback
            Console.WriteLine("Auto-selected Default model variant (fallback)");
            return ModelVariant.Default;
        }

        /// <summary>
        /// Get model path for specified variant
        /// </summary>
        public static string GetModelVariantPath(string executableFolder, ModelVariant variant)
        {
            string modelPath = variant switch
            {
                ModelVariant.FP32 => Path.Combine(executableFolder, "SqueezeNet.fp32.onnx"),
                ModelVariant.Default => Path.Combine(executableFolder, "SqueezeNet.onnx"),
                _ => Path.Combine(executableFolder, "SqueezeNet.onnx") // fallback
            };

            Console.WriteLine($"Using model variant: {variant} -> {modelPath}");
            return modelPath;
        }

        /// <summary>
        /// Resolve model paths with intelligent variant selection
        /// </summary>
        public static async Task<(string modelPath, string compiledModelPath, string labelsPath)> ResolvePaths(Options options, OrtEnv ortEnv)
        {
            string executableFolder = Path.GetDirectoryName(Assembly.GetEntryAssembly()!.Location)!;
            string modelPath;
            string labelsPath = Path.Combine(executableFolder, "SqueezeNet.Labels.txt");

            if (options.UseModelCatalog)
            {
                Console.WriteLine("Using model catalog...");
                
                // Build source
                string sampleCatalogJsonPath = Path.Combine(executableFolder, "SqueezeNetModelCatalog.json");
                var uri = new System.Uri(sampleCatalogJsonPath);
                var sampleCatalogSource = await ModelCatalogSource.CreateFromUriAsync(uri);
                
                ModelCatalog modelCatalog = new ModelCatalog(new[] { sampleCatalogSource });
                
                // Use intelligent model variant selection based on execution provider and device capabilities
                ModelVariant actualVariant = DetermineModelVariant(options, ortEnv);
                
                CatalogModelInfo modelFromCatalog;
                
                string modelVariantName = (actualVariant == ModelVariant.FP32) ? "squeezenet-fp32" : "squeezenet";
                
                modelFromCatalog = await modelCatalog.FindModelAsync(modelVariantName);
                
                if (modelFromCatalog != null)
                {
                    var additionalHeaders = new Dictionary<string, string>();
                    var catalogModelInstanceOp = modelFromCatalog.GetInstanceAsync(additionalHeaders);
                    
                    catalogModelInstanceOp.Progress += (operation, progress) => {
                        Console.Write($"Model download progress: {progress}%\r");
                    };
                    
                    var catalogModelInstanceResult = await catalogModelInstanceOp;
                    
                    if (catalogModelInstanceResult.Status == CatalogModelInstanceStatus.Available)
                    {
                        var catalogModelInstance = catalogModelInstanceResult.GetInstance();
                        var modelPaths = catalogModelInstance.ModelPaths;
                        
                        string modelFolderPath = modelPaths[0];
                        string modelName = $"{modelVariantName}.onnx";
                        modelPath = Path.Combine(modelFolderPath, modelName);
                        Console.WriteLine($"Using model from catalog at: {modelPath}");
                        
                        // Get labels
                        labelsPath = Path.Combine(modelFolderPath, "SqueezeNet.Labels.txt");
                    }
                    else
                    {
                        Console.WriteLine("Model download failed. Falling back to executableFolder");
                        modelPath = GetModelVariantPath(executableFolder, actualVariant);
                    }
                }
                else
                {
                    Console.WriteLine($"Model with alias or ID '{modelVariantName}' not found in catalog. Falling back to executableFolder");
                    modelPath = GetModelVariantPath(executableFolder, actualVariant);
                }
            }
            else if (!string.IsNullOrWhiteSpace(options.ModelPath))
            {
                // User provided custom model path - use it as-is
                modelPath = options.ModelPath.Contains(Path.DirectorySeparatorChar) ?
                    options.ModelPath : Path.Combine(executableFolder, options.ModelPath);
            }
            else
            {
                // Using default SqueezeNet model - determine the best variant based on EP configuration
                var variant = DetermineModelVariant(options, ortEnv);
                modelPath = GetModelVariantPath(executableFolder, variant);
            }

            string compiledModelPath = options.OutputPath.Contains(Path.DirectorySeparatorChar) ?
                options.OutputPath : Path.Combine(executableFolder, options.OutputPath);

            if (!File.Exists(labelsPath))
            {
                throw new FileNotFoundException($"Labels file not found: {labelsPath}");
            }

            return (modelPath, compiledModelPath, labelsPath);
        }

        /// <summary>
        /// Resolve model paths (legacy method for backward compatibility)
        /// </summary>
        public static (string modelPath, string compiledModelPath, string labelsPath) ResolvePaths(Options options)
        {
            string executableFolder = Path.GetDirectoryName(Assembly.GetEntryAssembly()!.Location)!;

            string modelPath;
            
            // Check if user specified a custom model path
            if (!string.IsNullOrWhiteSpace(options.ModelPath))
            {
                // User provided custom model path - use it as-is
                modelPath = options.ModelPath.Contains(Path.DirectorySeparatorChar) ?
                    options.ModelPath : Path.Combine(executableFolder, options.ModelPath);
            }
            else
            {
                // Using default SqueezeNet model - use the variant from options (will be Default)
                modelPath = GetModelVariantPath(executableFolder, options.Variant);
            }

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
            else if (!string.IsNullOrEmpty(options.EpName))
            {
                Console.WriteLine($"Using explicit execution provider: {options.EpName}");
                if (!ExecutionProviderManager.ConfigureSelectedExecutionProvider(sessionOptions,
                                                                                 ortEnv,
                                                                                 options.EpName,
                                                                                 options.DeviceType))
                {
                    throw new Exception("Failed to configure selected execution provider");
                }
            }
            else
            {
                throw new Exception("Could not find an EP selection policy or an explicit execution provider.");
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
                    else if (!string.IsNullOrEmpty(options.EpName))
                    {
                        if (!ExecutionProviderManager.ConfigureSelectedExecutionProvider(tempSessionOptions,
                                                                                          ortEnv,
                                                                                          options.EpName,
                                                                                          options.DeviceType))
                        {
                            throw new Exception("Failed to configure selected execution provider");
                        }
                    }
                    else
                    {
                        throw new Exception("Could not find an EP selection policy or an explicit execution provider.");
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
