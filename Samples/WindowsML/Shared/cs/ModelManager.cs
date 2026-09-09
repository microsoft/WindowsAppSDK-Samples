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
        /// Generate a device-specific compiled model path.
        /// Encodes EP policy/name, device type, and performance mode into the filename
        /// so that compiled models for different device configurations don't collide.
        /// </summary>
        public static string GenerateCompiledModelPath(string modelPath, string executableFolder, Options options)
        {
            // If user explicitly specified --compiled_output, use it as-is
            if (!string.IsNullOrEmpty(options.OutputPath))
            {
                return options.OutputPath.Contains(Path.DirectorySeparatorChar) ?
                    options.OutputPath : Path.Combine(executableFolder, options.OutputPath);
            }

            string baseName = Path.GetFileNameWithoutExtension(modelPath);
            string suffix = BuildDeviceSuffix(options);

            string fileName = $"{baseName}_ctx{suffix}.onnx";
            Console.WriteLine($"Compiled model path: {Path.Combine(executableFolder, fileName)}");
            return Path.Combine(executableFolder, fileName);
        }

        /// <summary>
        /// Build a device-identifying suffix for the compiled model filename.
        /// </summary>
        private static string BuildDeviceSuffix(Options options)
        {
            var parts = new List<string>();

            if (options.EpPolicy.HasValue)
            {
                parts.Add(options.EpPolicy.Value.ToString());
            }
            else if (!string.IsNullOrEmpty(options.EpName))
            {
                parts.Add(options.EpName);

                // Try to determine device type
                string? deviceType = options.DeviceType;

                if (!string.IsNullOrEmpty(deviceType))
                {
                    parts.Add(deviceType);
                }
            }

            if (options.PerfMode != PerformanceMode.Default)
            {
                parts.Add(options.PerfMode.ToString());
            }

            return parts.Count > 0 ? "_" + string.Join("_", parts) : "";
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

                // Use intelligent model variant selection based on execution provider and device capabilities
                ModelVariant actualVariant = DetermineModelVariant(options, ortEnv);
                if (File.Exists(sampleCatalogJsonPath))
                {
                    var uri = new System.Uri(sampleCatalogJsonPath);
                    var sampleCatalogSource = await ModelCatalogSource.CreateFromUriAsync(uri);
                    ModelCatalog modelCatalog = new ModelCatalog(new[] { sampleCatalogSource });
                    CatalogModelInfo modelFromCatalog;
                    string modelVariantName = (actualVariant == ModelVariant.FP32) ? "squeezenet-fp32" : "squeezenet";
                    modelFromCatalog = await modelCatalog.FindModelAsync(modelVariantName);
                    if (modelFromCatalog != null)
                    {
                        var additionalHeaders = new Dictionary<string, string>();
                        var catalogModelInstanceOp = modelFromCatalog.GetInstanceAsync(additionalHeaders);

                        catalogModelInstanceOp.Progress += (operation, progress) =>
                        {
                            Console.Write($"Model download progress: {progress}%\r");
                        };

                        var catalogModelInstanceResult = await catalogModelInstanceOp;

                        if (catalogModelInstanceResult.Status == CatalogModelInstanceStatus.Available)
                        {
                            using var catalogModelInstance = catalogModelInstanceResult.GetInstance();
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
                else
                {
                    Console.WriteLine("Model catalog JSON file not found. Falling back to executableFolder");
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

            string compiledModelPath = GenerateCompiledModelPath(modelPath, executableFolder, options);

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

            string compiledModelPath = GenerateCompiledModelPath(modelPath, executableFolder, options);

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
                                                                                 options.DeviceType,
                                                                                 options.PerfMode))
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
            TryCompileModel(sessionOptions, modelPath, compiledModelPath);
        }

        private static bool TryCompileModel(SessionOptions sessionOptions, string modelPath, string compiledModelPath)
        {
            Console.WriteLine($"Compiling model from {modelPath}");
            Console.WriteLine($"Output path: {compiledModelPath}");

            // Create compilation options from session options
            using OrtModelCompilationOptions compileOptions = new(sessionOptions);

            try
            {
                // Set input and output model paths
                compileOptions.SetInputModelPath(modelPath);
                compileOptions.SetOutputModelPath(compiledModelPath);

                // Keep EP context binaries in the ONNX file so temporary-file replacement
                // cannot strand a compiled model that references a temp-named sidecar.
                // External initializer output is not configured, so initializers also remain inline.
                compileOptions.SetEpContextEmbedMode(true);

                Console.WriteLine("Starting compile, this may take a few moments...");
                DateTime start = DateTime.Now;

                // Compile the model
                compileOptions.CompileModel();

                TimeSpan duration = DateTime.Now - start;
                Console.WriteLine($"Model compiled successfully in {duration.TotalMilliseconds} ms");
                return true;
            }
            catch (Exception ex)
            {
                Console.Error.WriteLine($"Failed to compile model: {ex.Message}");
                return false;
            }
        }

        /// <summary>
        /// Determine actual model path to use (with compilation logic)
        /// </summary>
        public static string ResolveActualModelPath(Options options, string modelPath, string compiledModelPath, OrtEnv ortEnv)
        {
            bool isCompiledModelAvailable = File.Exists(compiledModelPath);

            if (isCompiledModelAvailable)
            {
                bool foundCompatibilityMetadata = false;
                bool foundNonOptimalCompatibility = false;
                bool compatibilityCheckFailed = false;
                Console.WriteLine($"Checking compiled model compatibility: {compiledModelPath}");

                try
                {
                    IReadOnlyList<OrtEpDevice> discoveredDevices = ortEnv.GetEpDevices();
                    List<(string EpName, List<OrtEpDevice> Devices)> candidateGroups =
                        SelectCompatibilityDeviceGroups(discoveredDevices, options);

                    if (candidateGroups.Count == 0)
                    {
                        Console.WriteLine(!string.IsNullOrWhiteSpace(options.EpName)
                            ? $"  No discovered devices match EP '{options.EpName}'" +
                              (!string.IsNullOrWhiteSpace(options.DeviceType)
                                  ? $" and device type '{options.DeviceType}'."
                                  : ".")
                            : "  No execution provider devices matched the configured EP policy.");
                    }

                    foreach ((string epName, List<OrtEpDevice> devices) in candidateGroups)
                    {
                        Console.WriteLine(
                            $"  Probing EP '{epName}' with device(s): {FormatDeviceContext(devices)}");

                        try
                        {
                            string compatibilityInfo =
                                ortEnv.GetCompatibilityInfoFromModel(compiledModelPath, epName);
                            if (string.IsNullOrWhiteSpace(compatibilityInfo))
                            {
                                Console.WriteLine(
                                    $"  Compiled model has no compatibility metadata for EP '{epName}'.");
                                continue;
                            }

                            foundCompatibilityMetadata = true;
                            OrtCompiledModelCompatibility compatibilityStatus =
                                ortEnv.GetModelCompatibilityForEpDevices(devices, compatibilityInfo);
                            Console.WriteLine($"  EP '{epName}' compatibility status: {compatibilityStatus}.");

                            if (compatibilityStatus == OrtCompiledModelCompatibility.EP_SUPPORTED_OPTIMAL)
                            {
                                Console.WriteLine(
                                    $"  Compiled model is optimal for EP '{epName}'.");
                            }
                            else
                            {
                                foundNonOptimalCompatibility = true;
                            }
                        }
                        catch (Exception ex)
                        {
                            compatibilityCheckFailed = true;
                            Console.WriteLine(
                                $"  Compatibility check failed for EP '{epName}': {ex.Message}");
                        }
                    }
                }
                catch (Exception ex)
                {
                    compatibilityCheckFailed = true;
                    Console.WriteLine($"  Compiled model compatibility check failed: {ex.Message}");
                }

                bool canReuseCompiledModel =
                    foundCompatibilityMetadata &&
                    !foundNonOptimalCompatibility &&
                    !compatibilityCheckFailed;

                if (canReuseCompiledModel)
                {
                    Console.WriteLine(
                        "All applicable compatibility metadata reports EP_SUPPORTED_OPTIMAL.");
                    Console.WriteLine($"Using existing compiled model: {compiledModelPath}");
                    return compiledModelPath;
                }

                if (!foundCompatibilityMetadata)
                {
                    Console.WriteLine(
                        "No compatibility metadata matched the selected execution provider devices.");
                }
                else if (foundNonOptimalCompatibility)
                {
                    Console.WriteLine(
                        "At least one applicable execution provider group reported a non-optimal status.");
                }

                Console.WriteLine($"Existing compiled model is not optimal and will not be used: {compiledModelPath}");

                if (!options.Compile)
                {
                    Console.WriteLine($"Using original model: {modelPath}");
                    return modelPath;
                }
            }

            if (options.Compile)
            {
                Console.WriteLine(isCompiledModelAvailable
                    ? "Attempting to replace the non-optimal compiled model"
                    : "No compiled model found, attempting to create compiled model");

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
                                                                                          options.DeviceType,
                                                                                          options.PerfMode))
                        {
                            throw new Exception("Failed to configure selected execution provider");
                        }
                    }
                    else
                    {
                        throw new Exception("Could not find an EP selection policy or an explicit execution provider.");
                    }

                    if (TryCompileAndReplaceModel(
                        tempSessionOptions,
                        modelPath,
                        compiledModelPath))
                    {
                        Console.WriteLine($"Compiled model created successfully at {compiledModelPath}");
                        return compiledModelPath;
                    }
                }
                catch (Exception ex)
                {
                    Console.WriteLine($"Compilation failed: {ex.Message}");
                }

                Console.WriteLine($"Falling back to original model: {modelPath}");
                return modelPath;
            }

            Console.WriteLine($"Using original model: {modelPath}");
            return modelPath;
        }

        private static bool TryCompileAndReplaceModel(
            SessionOptions sessionOptions,
            string modelPath,
            string compiledModelPath)
        {
            string fullModelPath = Path.GetFullPath(modelPath);
            string fullCompiledModelPath = Path.GetFullPath(compiledModelPath);

            if (fullModelPath.Equals(fullCompiledModelPath, StringComparison.OrdinalIgnoreCase))
            {
                Console.WriteLine("Compilation output path must be different from the original model path.");
                return false;
            }

            string outputDirectory = Path.GetDirectoryName(fullCompiledModelPath)!;
            Directory.CreateDirectory(outputDirectory);

            string temporaryCompiledModelPath = Path.Combine(
                outputDirectory,
                $".{Path.GetFileNameWithoutExtension(fullCompiledModelPath)}.{Guid.NewGuid():N}.tmp.onnx");

            try
            {
                if (!TryCompileModel(sessionOptions, fullModelPath, temporaryCompiledModelPath) ||
                    !File.Exists(temporaryCompiledModelPath))
                {
                    Console.WriteLine("Compilation did not produce a usable temporary model.");
                    return false;
                }

                if (File.Exists(fullCompiledModelPath))
                {
                    // The temporary model is in the same directory, so replacement is same-volume.
                    File.Replace(
                        temporaryCompiledModelPath,
                        fullCompiledModelPath,
                        destinationBackupFileName: null,
                        ignoreMetadataErrors: true);
                }
                else
                {
                    File.Move(temporaryCompiledModelPath, fullCompiledModelPath);
                }
                return true;
            }
            catch (Exception ex)
            {
                Console.WriteLine(
                    $"Failed to replace compiled model cache '{fullCompiledModelPath}' " +
                    $"with temporary output '{temporaryCompiledModelPath}': {ex.Message}");
                return false;
            }
            finally
            {
                TryDeleteFile(temporaryCompiledModelPath);
            }
        }

        private static List<(string EpName, List<OrtEpDevice> Devices)> SelectCompatibilityDeviceGroups(
            IReadOnlyList<OrtEpDevice> discoveredDevices,
            Options options)
        {
            List<(string EpName, List<OrtEpDevice> Devices)> groups = discoveredDevices
                .GroupBy(device => device.EpName, StringComparer.OrdinalIgnoreCase)
                .Select(group => (group.Key, group.ToList()))
                .ToList();

            if (!string.IsNullOrWhiteSpace(options.EpName))
            {
                (string EpName, List<OrtEpDevice> Devices) group = groups.FirstOrDefault(
                    candidate => candidate.EpName.Equals(options.EpName, StringComparison.OrdinalIgnoreCase));
                if (group.Devices == null)
                {
                    return [];
                }

                if (string.IsNullOrWhiteSpace(options.DeviceType))
                {
                    return [group];
                }

                // Explicit session configuration selects the first matching device.
                OrtEpDevice? device = group.Devices.FirstOrDefault(
                    candidate => candidate.HardwareDevice.Type.ToString().Equals(
                        options.DeviceType,
                        StringComparison.OrdinalIgnoreCase));
                return device == null ? [] : [(group.EpName, [device])];
            }

            HashSet<string> policyDeviceTypes = GetPolicyDeviceTypes(
                options.EpPolicy ?? ExecutionProviderDevicePolicy.DEFAULT);

            // Preserve EP grouping while including the policy's preferred hardware and CPU fallback.
            return groups
                .Select(group => (
                    group.EpName,
                    group.Devices.Where(device => policyDeviceTypes.Contains(
                        device.HardwareDevice.Type.ToString())).ToList()))
                .Where(group => group.Item2.Count > 0)
                .Select(group => (group.EpName, group.Item2))
                .ToList();
        }

        private static HashSet<string> GetPolicyDeviceTypes(ExecutionProviderDevicePolicy policy)
        {
            string? preferredDeviceType = policy switch
            {
                ExecutionProviderDevicePolicy.PREFER_NPU or
                ExecutionProviderDevicePolicy.MAX_EFFICIENCY or
                ExecutionProviderDevicePolicy.MIN_OVERALL_POWER => "NPU",
                ExecutionProviderDevicePolicy.PREFER_GPU or
                ExecutionProviderDevicePolicy.MAX_PERFORMANCE => "GPU",
                _ => null
            };

            var deviceTypes = new HashSet<string>(StringComparer.OrdinalIgnoreCase) { "CPU" };
            if (preferredDeviceType != null)
            {
                deviceTypes.Add(preferredDeviceType);
            }

            return deviceTypes;
        }

        private static string FormatDeviceContext(IEnumerable<OrtEpDevice> devices)
        {
            return string.Join(", ", devices.Select(device =>
            {
                OrtHardwareDevice hardwareDevice = device.HardwareDevice;
                return $"{hardwareDevice.Type} (vendor={hardwareDevice.Vendor}, " +
                       $"vendorId={hardwareDevice.VendorId}, deviceId={hardwareDevice.DeviceId})";
            }));
        }

        private static void TryDeleteFile(string path)
        {
            try
            {
                if (File.Exists(path))
                {
                    File.Delete(path);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Warning: Could not remove temporary model '{path}': {ex.Message}");
            }
        }
    }
}
