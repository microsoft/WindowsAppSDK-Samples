// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.

using System.Reflection;
using System.Runtime.InteropServices.WindowsRuntime;
using Microsoft.ML.OnnxRuntime;
using Microsoft.ML.OnnxRuntime.Tensors;
using Windows.Graphics.Imaging;
using Windows.Storage;
using Windows.Storage.Streams;

internal class Program
{
    private static async Task<SoftwareBitmap> LoadImageFileAsync(string filePath)
    {
        StorageFile file = await StorageFile.GetFileFromPathAsync(filePath);
        var stream = await file.OpenAsync(FileAccessMode.Read);
        BitmapDecoder decoder = await BitmapDecoder.CreateAsync(stream);
        SoftwareBitmap softwareBitmap = await decoder.GetSoftwareBitmapAsync();

        return softwareBitmap;
    }

    public static async Task<DenseTensor<float>> PreprocessImageAsync(SoftwareBitmap softwareBitmap)
    {
        const int targetWidth = 224;
        const int targetHeight = 224;

        float[] mean = new float[] { 0.485f, 0.456f, 0.406f };
        float[] std = new float[] { 0.229f, 0.224f, 0.225f };

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
        var buffer = new Windows.Storage.Streams.Buffer(bufferSize);
        softwareBitmap.CopyToBuffer(buffer);
        byte[] pixels = buffer.ToArray();

        // Output Tensor shape: [1, 3, 224, 224]
        var tensorData = new DenseTensor<float>(new[] { 1, 3, targetHeight, targetWidth });

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

    private static async Task<SoftwareBitmap> ResizeSoftwareBitmapAsync(SoftwareBitmap bitmap, int width, int height)
    {
        using (var stream = new InMemoryRandomAccessStream())
        {
            var encoder = await BitmapEncoder.CreateAsync(BitmapEncoder.PngEncoderId, stream);
            encoder.SetSoftwareBitmap(bitmap);
            encoder.IsThumbnailGenerated = false;
            await encoder.FlushAsync();
            stream.Seek(0);

            var decoder = await BitmapDecoder.CreateAsync(stream);
            var transform = new BitmapTransform()
            {
                ScaledWidth = (uint)width,
                ScaledHeight = (uint)height,
                InterpolationMode = BitmapInterpolationMode.Fant
            };
            var resized = await decoder.GetSoftwareBitmapAsync(
                BitmapPixelFormat.Bgra8,
                BitmapAlphaMode.Premultiplied,
                transform,
                ExifOrientationMode.IgnoreExifOrientation,
                ColorManagementMode.DoNotColorManage);

            return resized;
        }
    }

    private static IList<string> LoadLabels(string labelsPath)
    {
        return File.ReadAllLines(labelsPath)
            .Select(line => line.Split(',', 2)[1])
            .ToList();
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
    public static async Task<int> Main(string[] arguments)
    {
        try
        {
            // Create a new instance of EnvironmentCreationOptions
            EnvironmentCreationOptions envOptions = new()
            {
                logId = "ResnetBuildDemo",
                logLevel = OrtLoggingLevel.ORT_LOGGING_LEVEL_ERROR
            };

            // Pass the options by reference to CreateInstanceWithOptions
            OrtEnv ortEnv = OrtEnv.CreateInstanceWithOptions(ref envOptions);

            // Use WinML to download and register Execution Providers
            Microsoft.Windows.AI.MachineLearning.Infrastructure infrastructure = new();
            Console.WriteLine("Ensure EPs are downloaded ...");
            await infrastructure.DownloadPackagesAsync();
            await infrastructure.RegisterExecutionProviderLibrariesAsync();

            // Prepare paths
            string executableFolder = Path.GetDirectoryName(Assembly.GetEntryAssembly()!.Location)!;
            string labelsPath = Path.Combine(executableFolder, "ResNet50Labels.txt");
            string imagePath = Path.Combine(executableFolder, "dog.jpg");
            
            // TODO: Please use AITK Model Conversion tool to download and convert Resnet, and paste the converted path here
            string modelPath = @"";
            string compiledModelPath = @"";

            //Create Onnx session
            Console.WriteLine("Creating session ...");
            var sessionOptions = new SessionOptions();
            // Set EP Selection Policy
            sessionOptions.SetEpSelectionPolicy(ExecutionProviderDevicePolicy.MIN_OVERALL_POWER);

            // Compile the model if not already compiled
            bool isCompiled = File.Exists(compiledModelPath);
            if (!isCompiled)
            {
                Console.WriteLine("No compiled model found. Compiling model ...");
                using (var compileOptions = new OrtModelCompilationOptions(sessionOptions))
                {
                    compileOptions.SetInputModelPath(modelPath);
                    compileOptions.SetOutputModelPath(compiledModelPath);
                    compileOptions.CompileModel();
                    isCompiled = File.Exists(compiledModelPath);
                    if (isCompiled)
                    {
                        Console.WriteLine("Model compiled successfully!");
                    }
                    else
                    {
                        Console.WriteLine("Failed to compile the model. Will use original model.");
                    }
                }
            }
            else
            {
                Console.WriteLine("Found precompiled model.");
            }
            var modelPathToUse = isCompiled ? compiledModelPath : modelPath;
            using var session = new InferenceSession(modelPathToUse, sessionOptions);

            Console.WriteLine("Preparing input ...");
            // Load and preprocess image
            var input = await PreprocessImageAsync(await LoadImageFileAsync(imagePath));
            // Prepare input tensor
            var inputName = session.InputMetadata.First().Key;
            var inputTensor = new DenseTensor<float>(
                input.ToArray(),          // Use the DenseTensor<float> directly
                new[] { 1, 3, 224, 224 }, // Shape of the tensor
                false                     // isReversedStride should be explicitly set to false
            );

            // Bind inputs and run inference
            var inputs = new List<NamedOnnxValue>
            {
                NamedOnnxValue.CreateFromTensor(inputName, inputTensor)
            };

            Console.WriteLine("Running inference ...");
            var results = session.Run(inputs);
            for (int i = 0; i < 40; i++)
            {
                results = session.Run(inputs);
            }

            // Extract output tensor
            var outputName = session.OutputMetadata.First().Key;
            var resultTensor = results.First(r => r.Name == outputName).AsEnumerable<float>().ToArray();

            // Load labels and print results
            var labels = LoadLabels(labelsPath);
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
