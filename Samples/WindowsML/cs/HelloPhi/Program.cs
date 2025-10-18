// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.ML.OnnxRuntimeGenAI;
using Microsoft.Windows.AI.MachineLearning;

using System.Text;
using System.Text.Json;
using Windows.Foundation;

static async Task InitializeProvidersAsync(bool allowDownload)
{
    Console.WriteLine("Getting available providers...");
    var catalog = ExecutionProviderCatalog.GetDefault();
    var providers = catalog.FindAllProviders();

    if (providers is null || providers.Length == 0)
    {
        Console.WriteLine("No execution providers found in catalog.");
        return;
    }

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
                Console.WriteLine($"  EnsureReadyAsync: {provider.Name}");
                await provider.EnsureReadyAsync();
            }
        
            var registerResult = provider.TryRegister();
            Console.WriteLine($"  TryRegister: {registerResult}");
        }
        catch (Exception ex)
        {
            Console.WriteLine($"  Failed to initialize provider {provider.Name}: {ex.Message}");
            // Continue with other providers
        }
    }
}

static void PrintUsage()
{
    Console.WriteLine("Usage:");
    Console.WriteLine("  -m model_path");
    Console.WriteLine("\t\t\t\tPath to the model");
    Console.WriteLine("  --non-interactive (optional)");
    Console.WriteLine("\t\t\t\tInteractive mode");
    Console.WriteLine("  -use_model_catalog (optional)");
    Console.WriteLine("\t\t\t\tUse ModelCatalog APIs to get the model path");
}

using OgaHandle ogaHandle = new();

if (args.Length < 1)
{
    PrintUsage();
    Environment.Exit(-1);
}

bool interactive = true;
string modelPath = string.Empty;
bool useModelCatalog = false;

uint i = 0;
while (i < args.Length)
{
    string arg = args[i];
    if (arg == "--non-interactive")
    {
        interactive = false;
    }
    else if (arg == "-m")
    {
        if (i + 1 < args.Length)
        {
            modelPath = Path.Combine(args[i + 1]);
        }
        i++;
    }
    else if (arg == "-use_model_catalog")
    {
        useModelCatalog = true;
    }
    i++;
}

// If useModelCatalog is true, then we will use ModelCatalog APIs to get the model path
if (useModelCatalog)
{
    try
    {
        string catalogPath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "SampleModelCatalog.json");
        Console.WriteLine($"Using model catalog: {catalogPath}");
        
        var catalogSource = await ModelCatalogSource.CreateFromUriAsync(new Uri(catalogPath));
        var modelCatalog = new ModelCatalog(new[] { catalogSource });

        // Use the model ID from SampleModelCatalog.json
        Console.WriteLine("Finding model in catalog...");
        var modelInfo = modelCatalog.FindModelAsync("phi-4-mini-instruct-onnx").Get();
        
        // Show progress for GetInstanceAsync operation
        Console.WriteLine("Getting model instance from catalog:");
        
        // Setup progress handler for GetInstanceAsync
        var getInstanceOperation = modelInfo.GetInstanceAsync(new Dictionary<string, string>());
        getInstanceOperation.Progress = (sender, progressValue) => 
        {
            // Simple progress display with percentage
            int percent = (int)(progressValue * 100);
            Console.Write($"\rProgress: {percent}%");
        };
        
        var resultInfo = await getInstanceOperation;
        Console.WriteLine("\rCompleted getting model instance.                ");
        
        var catalogModelInstance = resultInfo.GetInstance();
        
        if (catalogModelInstance.ModelPaths.Count > 0)
        {
            modelPath = catalogModelInstance.ModelPaths[0];
            Console.WriteLine($"Found model path from catalog: {modelPath}");
        }
        else
        {
            throw new Exception("No model paths found in catalog model instance.");
        }
    }
    catch (Exception ex)
    {
        Console.WriteLine($"Failed to get model from catalog: {ex.Message}");
        Environment.Exit(-1);
    }
}

if (string.IsNullOrEmpty(modelPath))
{
    throw new Exception("Model path must be specified either directly with -m or by using -use_model_catalog");
}

Console.WriteLine("-------------");
Console.WriteLine("Hello, Phi!");
Console.WriteLine("-------------");

Console.WriteLine("Model path: " + modelPath);
Console.WriteLine("Interactive: " + interactive);

InitializeProvidersAsync(allowDownload: true).Wait();

using Config config = new(modelPath);
using Model model = new(config);
using Tokenizer tokenizer = new(model);

int option = 2;
if (interactive)
{
    Console.WriteLine("Please enter option number:");
    Console.WriteLine("1. Complete Q&A");
    Console.WriteLine("2. Streaming Q&A");
    _ = int.TryParse(Console.ReadLine(), out option);
}

int minLength = 50;
int maxLength = 500;

static string GetPrompt(bool interactive)
{
    string prompt = "def is_prime(num):"; // Example prompt
    if (interactive)
    {
        Console.WriteLine("Prompt: (Use quit() to exit)");
        prompt = Console.ReadLine() ?? string.Empty;
    }
    return prompt;
}

static string BuildChatTemplateMessages(string prompt)
{
    using var stream = new MemoryStream();
    using var writer = new Utf8JsonWriter(stream);

    writer.WriteStartArray();

    // System message
    writer.WriteStartObject();
    writer.WriteString("role", "system");
    writer.WriteString("content", "You are a helpful AI assistant.");
    writer.WriteEndObject();

    // User message
    writer.WriteStartObject();
    writer.WriteString("role", "user");
    writer.WriteString("content", prompt);
    writer.WriteEndObject();

    writer.WriteEndArray();
    writer.Flush();

    return Encoding.UTF8.GetString(stream.ToArray());
}

do
{
    string prompt = GetPrompt(interactive);
    if (string.IsNullOrEmpty(prompt))
    {
        continue;
    }
    if (string.Compare(prompt, "quit()", StringComparison.OrdinalIgnoreCase) == 0)
    {
        break;
    }
    string messages = BuildChatTemplateMessages(prompt);

    Sequences sequences = tokenizer.Encode(tokenizer.ApplyChatTemplate("", messages, "", true));

    if (option == 1) // Complete Output
    {
        using GeneratorParams generatorParams = new(model);
        generatorParams.SetSearchOption("min_length", minLength);
        generatorParams.SetSearchOption("max_length", maxLength);
        using Generator generator = new(model, generatorParams);
        generator.AppendTokenSequences(sequences);
        System.Diagnostics.Stopwatch watch = System.Diagnostics.Stopwatch.StartNew();
        
        // Fix for CS9202 error
        bool isDone = false;
        while (!isDone)
        {
            generator.GenerateNextToken();
            isDone = generator.IsDone();
        }

        int[] outputSequence = new int[generator.GetSequence(0).Length];
        generator.GetSequence(0).CopyTo(outputSequence);
        
        string outputString = tokenizer.Decode(outputSequence);
        watch.Stop();
        double runTimeInSeconds = watch.Elapsed.TotalSeconds;
        Console.WriteLine("Output:");
        Console.WriteLine(outputString);
        int totalTokens = outputSequence.Length;
        Console.WriteLine($"Tokens: {totalTokens} Time: {runTimeInSeconds:0.00} Tokens per second: {totalTokens / runTimeInSeconds:0.00}");
    }
    else if (option == 2) //Streaming Output
    {
        using GeneratorParams generatorParams = new(model);
        generatorParams.SetSearchOption("min_length", minLength);
        generatorParams.SetSearchOption("max_length", maxLength);
        using TokenizerStream tokenizerStream = tokenizer.CreateStream();
        using Generator generator = new(model, generatorParams);
        generator.AppendTokenSequences(sequences);
        System.Diagnostics.Stopwatch watch = System.Diagnostics.Stopwatch.StartNew();
        
        // Fix for CS9202 error
        bool isDone = false;
        while (!isDone)
        {
            generator.GenerateNextToken();
            int lastToken = generator.GetSequence(0)[generator.GetSequence(0).Length - 1];
            Console.Write(tokenizerStream.Decode(lastToken));
            isDone = generator.IsDone();
        }
        
        Console.WriteLine();
        watch.Stop();
        double runTimeInSeconds = watch.Elapsed.TotalSeconds;
        
        int[] outputSequence = new int[generator.GetSequence(0).Length];
        generator.GetSequence(0).CopyTo(outputSequence);
        
        int totalTokens = outputSequence.Length;
        Console.WriteLine($"Streaming Tokens: {totalTokens} Time: {runTimeInSeconds:0.00} Tokens per second: {totalTokens / runTimeInSeconds:0.00}");
    }
} while (interactive);
