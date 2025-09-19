// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.ML.OnnxRuntimeGenAI;

static void PrintUsage()
{
    Console.WriteLine("Usage:");
    Console.WriteLine("  -m model_path");
    Console.WriteLine("\t\t\t\tPath to the model");
    Console.WriteLine("  --non-interactive (optional)");
    Console.WriteLine("\t\t\t\tInteractive mode");
}

using OgaHandle ogaHandle = new();

if (args.Length < 1)
{
    PrintUsage();
    Environment.Exit(-1);
}

bool interactive = true;
string modelPath = string.Empty;

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
    }
    i++;
}

if (string.IsNullOrEmpty(modelPath))
{
    throw new Exception("Model path must be specified");
}

Console.WriteLine("-------------");
Console.WriteLine("Hello, Phi!");
Console.WriteLine("-------------");

Console.WriteLine("Model path: " + modelPath);
Console.WriteLine("Interactive: " + interactive);

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
        prompt = Console.ReadLine();
    }
    return prompt;
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
    string messages = $@"[{{""role"":""system"",""content"":""You are a helpful AI assistant.""}},{{""role"":""user"",""content"":""{prompt}""}}]";
    Sequences sequences = tokenizer.Encode(tokenizer.ApplyChatTemplate("", messages, "", true));

    if (option == 1) // Complete Output
    {
        using GeneratorParams generatorParams = new(model);
        generatorParams.SetSearchOption("min_length", minLength);
        generatorParams.SetSearchOption("max_length", maxLength);
        using Generator generator = new(model, generatorParams);
        generator.AppendTokenSequences(sequences);
        System.Diagnostics.Stopwatch watch = System.Diagnostics.Stopwatch.StartNew();
        while (!generator.IsDone())
        {
            generator.GenerateNextToken();
        }

        ReadOnlySpan<int> outputSequence = generator.GetSequence(0);
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
        while (!generator.IsDone())
        {
            generator.GenerateNextToken();
            Console.Write(tokenizerStream.Decode(generator.GetSequence(0)[^1]));
        }
        Console.WriteLine();
        watch.Stop();
        double runTimeInSeconds = watch.Elapsed.TotalSeconds;
        ReadOnlySpan<int> outputSequence = generator.GetSequence(0);
        int totalTokens = outputSequence.Length;
        Console.WriteLine($"Streaming Tokens: {totalTokens} Time: {runTimeInSeconds:0.00} Tokens per second: {totalTokens / runTimeInSeconds:0.00}");
    }
} while (interactive);
