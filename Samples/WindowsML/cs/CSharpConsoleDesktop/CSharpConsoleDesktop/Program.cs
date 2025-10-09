// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.

using Microsoft.ML.OnnxRuntime;

using WindowsML.Shared;

internal class Program
{
    private static async Task<int> Main(string[] arguments)
    {
        try
        {
            // Parse command-line arguments using shared helper
            Options options = ArgumentParser.ParseOptions(arguments);

            // Create ONNX Runtime environment using shared helper
            using var ortEnv = ModelManager.CreateEnvironment("CSharpConsoleDesktop");

            // Initialize execution providers using shared helper  
            await ModelManager.InitializeExecutionProvidersAsync(options.Download);

            // Resolve file paths using shared helper with intelligent model variant selection
            // This now supports model catalog functionality when options.UseModelCatalog is true
            var (modelPath, compiledModelPath, labelsPath) = await ModelManager.ResolvePaths(options, ortEnv);
            Console.WriteLine($"Image path: {options.ImagePath}");

            // Determine actual model path (with compilation support) using shared helper
            string actualModelPath = ModelManager.ResolveActualModelPath(options, modelPath, compiledModelPath, ortEnv);

            // Create inference session with EP configuration using shared helper
            using InferenceSession session = ModelManager.CreateSession(actualModelPath, options, ortEnv);

            // Load and preprocess image using shared helper
            using var videoFrame = await ImageProcessor.LoadImageFileAsync(options.ImagePath);
            var inputTensor = await ImageProcessor.PreprocessImageAsync(videoFrame);

            // Run inference using shared helper
            using var results = InferenceEngine.RunInference(session, inputTensor);
            var resultTensor = InferenceEngine.ExtractResults(session, results);

            // Process and display results using shared helper
            var labels = ResultProcessor.LoadLabels(labelsPath);
            ResultProcessor.PrintResults(labels, resultTensor);

            return 0;
        }
        catch (Exception ex)
        {
            Console.Error.WriteLine($"Exception: {ex}");
        }

        return -1;
    }
}