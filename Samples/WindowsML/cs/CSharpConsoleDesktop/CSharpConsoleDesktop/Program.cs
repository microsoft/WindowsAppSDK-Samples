// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.
using System.Reflection;
using System.Runtime.InteropServices.WindowsRuntime;

using Microsoft.Windows.AI.MachineLearning;
using Microsoft.ML.OnnxRuntime;
using Microsoft.ML.OnnxRuntime.Tensors;

using Windows.Graphics.Imaging;
using Windows.Media;
using Windows.Storage;
using Windows.Storage.Streams;

internal class Program
{
    private class Options
    {
        public ExecutionProviderDevicePolicy? EpPolicy { get; set; } = null; // Default: null (DISABLE)
        public bool Compile { get; set; } = false;
        public bool Download { get; set; } = false;
        public string ModelPath { get; set; } = "SqueezeNet.onnx"; // Default: SqueezeNet.onnx
        public string OutputPath { get; set; } = "SqueezeNet_ctx.onnx"; // Default: SqueezeNet_ctx.onnx
        public string ImagePath { get; set; } = string.Empty; // Mandatory field
    }

    private static Options ParseOptions(string[] args)
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

    private static void PrintHelp()
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

    private static async Task<VideoFrame> LoadImageFileAsync(string filePath)
    {
        try
        {
            StorageFile file = await StorageFile.GetFileFromPathAsync(filePath);
            IRandomAccessStream stream = await file.OpenAsync(FileAccessMode.Read);
            BitmapDecoder decoder = await BitmapDecoder.CreateAsync(stream);
            SoftwareBitmap softwareBitmap = await decoder.GetSoftwareBitmapAsync();
            VideoFrame inputImage = VideoFrame.CreateWithSoftwareBitmap(softwareBitmap);

            return inputImage;
        }
        catch (FileNotFoundException)
        {
            throw new FileNotFoundException($"Image file not found: {filePath}");
        }
        catch (Exception ex)
        {
            throw new Exception($"Error loading image: {ex.Message}", ex);
        }
    }

    private static void PrintResults(IList<string> labels, IReadOnlyList<float> results)
    {
        // Apply softmax to the results
        float maxLogit = results.Max();
        var expScores = results.Select(r => MathF.Exp(r - maxLogit)).ToList(); // stability with maxLogit
        float sumExp = expScores.Sum();
        var softmaxResults = expScores.Select(e => e / sumExp).ToList();

        // Get top 5 results
        IEnumerable<(int Index, float Confidence)> topResults = softmaxResults
            .Select((value, index) => (Index: index, Confidence: value))
            .OrderByDescending(x => x.Confidence)
            .Take(5);

        // Display results
        Console.WriteLine("Top Predictions:");
        Console.WriteLine("-------------------------------------------");
        Console.WriteLine("{0,-32} {1,10}", "Label", "Confidence");
        Console.WriteLine("-------------------------------------------");

        foreach (var result in topResults)
        {
            Console.WriteLine("{0,-32} {1,10:P2}", labels[result.Index], result.Confidence);
        }

        Console.WriteLine("-------------------------------------------");
    }

    private static IList<string> LoadLabels(string labelsPath)
    {
        return File.ReadAllLines(labelsPath)
            .Select(line => line.Split(',', 2)[1])
            .ToList();
    }

    private static async Task<DenseTensor<float>> PreprocessImageAsync(VideoFrame videoFrame)
    {
        SoftwareBitmap softwareBitmap = videoFrame.SoftwareBitmap;
        const int targetWidth = 224;
        const int targetHeight = 224;

        float[] mean = [0.485f, 0.456f, 0.406f];
        float[] std = [0.229f, 0.224f, 0.225f];

        // Convert to BGRA8
        if (softwareBitmap.BitmapPixelFormat != BitmapPixelFormat.Bgra8 ||
            softwareBitmap.BitmapAlphaMode != BitmapAlphaMode.Premultiplied)
        {
            softwareBitmap = SoftwareBitmap.Convert(softwareBitmap, BitmapPixelFormat.Bgra8, BitmapAlphaMode.Premultiplied);
        }

        // Resize
        softwareBitmap = await ResizeSoftwareBitmapAsync(softwareBitmap, targetWidth, targetHeight);

        // Get pixel data
        uint bufferSize = (uint)(targetWidth * targetHeight * 4);
        Windows.Storage.Streams.Buffer buffer = new(bufferSize);
        softwareBitmap.CopyToBuffer(buffer);
        byte[] pixels = buffer.ToArray();

        // Output Tensor shape: [1, 3, 224, 224]
        DenseTensor<float> tensorData = new([1, 3, targetHeight, targetWidth]);

        for (int y = 0; y < targetHeight; y++)
        {
            for (int x = 0; x < targetWidth; x++)
            {
                int pixelIndex = (y * targetWidth + x) * 4;
                float r = pixels[pixelIndex + 2] / 255f;
                float g = pixels[pixelIndex + 1] / 255f;
                float b = pixels[pixelIndex + 0] / 255f;

                // Normalize using mean/stddev
                r = (r - mean[0]) / std[0];
                g = (g - mean[1]) / std[1];
                b = (b - mean[2]) / std[2];

                int baseIndex = y * targetWidth + x;
                tensorData[0, 0, y, x] = r; // R
                tensorData[0, 1, y, x] = g; // G
                tensorData[0, 2, y, x] = b; // B
            }
        }

        return tensorData;
    }

    private static void PrintDeviceInfo(Dictionary<string, List<OrtEpDevice>> epDeviceMap)
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

    private static void ConfigureExecutionProviders(SessionOptions sessionOptions, OrtEnv environment)
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
                    // Configure threading for OpenVINO EP
                    epOptions["num_of_threads"] = "4";
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

    private static void CompileModel(SessionOptions sessionOptions, string modelPath, string compiledModelPath)
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

    private static async Task<SoftwareBitmap> ResizeSoftwareBitmapAsync(SoftwareBitmap bitmap, int width, int height)
    {
        using InMemoryRandomAccessStream stream = new();
        BitmapEncoder encoder = await BitmapEncoder.CreateAsync(BitmapEncoder.PngEncoderId, stream);
        encoder.SetSoftwareBitmap(bitmap);
        encoder.IsThumbnailGenerated = false;
        await encoder.FlushAsync();
        stream.Seek(0);

        BitmapDecoder decoder = await BitmapDecoder.CreateAsync(stream);
        BitmapTransform transform = new()
        {
            ScaledWidth = (uint)width,
            ScaledHeight = (uint)height,
            InterpolationMode = BitmapInterpolationMode.Fant
        };
        SoftwareBitmap resized = await decoder.GetSoftwareBitmapAsync(
            BitmapPixelFormat.Bgra8,
            BitmapAlphaMode.Premultiplied,
            transform,
            ExifOrientationMode.IgnoreExifOrientation,
            ColorManagementMode.DoNotColorManage);

        stream.Dispose();
        return resized;
    }

    private static async Task<int> Main(string[] arguments)
    {
        try
        {
            // Parse command-line arguments
            Options options = ParseOptions(arguments);

            // Create a new instance of EnvironmentCreationOptions
            EnvironmentCreationOptions envOptions = new()
            {
                logId = "CSharpConsoleDesktop",
                logLevel = OrtLoggingLevel.ORT_LOGGING_LEVEL_WARNING
            };

            // Pass the options by reference to CreateInstanceWithOptions
            using OrtEnv ortEnv = OrtEnv.CreateInstanceWithOptions(ref envOptions);

            Infrastructure infrastructure = new();

            // Only download packages if the download option is enabled
            if (options.Download)
            {
                try
                {
                    Console.WriteLine("Downloading packages ...");
                    await infrastructure.DownloadPackagesAsync();
                }
                catch (Exception ex)
                {
                    Console.WriteLine($"WARNING: Failed to download packages: {ex.Message}");
                }
            }

            await infrastructure.RegisterExecutionProviderLibrariesAsync();

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
            Console.WriteLine($"Image path: {options.ImagePath}");

            Console.WriteLine("Creating session ...");
            SessionOptions sessionOptions = new();

            // The platform allows the caller to specify what EP should be preferred for executing the model graph. There
            // are fallbacks in place if the desired hardware is not present on the device. For example, CPU is always
            // available to guarantee the model can execute. In this example, the caller expresses preference for the NPU,
            // with a fallback to the CPU if portions of the graph cannot be executed by the avaialble NPU EP.

            // Set EP selection policy from options
            if (options.EpPolicy.HasValue)
            {
                Console.WriteLine($"Using EP selection policy: {options.EpPolicy.Value}");
                sessionOptions.SetEpSelectionPolicy(options.EpPolicy.Value);
            }
            else
            {
                Console.WriteLine("Using explicit EP configuration");
                ConfigureExecutionProviders(sessionOptions, ortEnv);
            }

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
                    CompileModel(sessionOptions, modelPath, compiledModelPath);

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

            using InferenceSession session = new(actualModelPath, sessionOptions);

            Console.WriteLine("Preparing input ...");
            DenseTensor<float> input = await PreprocessImageAsync(await LoadImageFileAsync(options.ImagePath));

            // Prepare input tensor
            string inputName = session.InputMetadata.First().Key;
            DenseTensor<float> inputTensor = new(
                input.ToArray(),
                [1, 3, 224, 224], // Shape of the tensor
                false             // isReversedStride should be explicitly set to false
            );

            // Bind inputs and run inference
            List<NamedOnnxValue> inputs = [NamedOnnxValue.CreateFromTensor(inputName, inputTensor)];

            Console.WriteLine("Running inference ...");
            using IDisposableReadOnlyCollection<DisposableNamedOnnxValue> results = session.Run(inputs);

            // Extract output tensor
            string outputName = session.OutputMetadata.First().Key;
            float[] resultTensor = results.First(r => r.Name == outputName).AsEnumerable<float>().ToArray();

            // Load labels and print results
            IList<string> labels = LoadLabels(labelsPath);
            PrintResults(labels, resultTensor);

            return 0;
        }
        catch (Exception ex)
        {
            Console.Error.WriteLine($"Exception: {ex}");
        }

        return -1;
    }
}
