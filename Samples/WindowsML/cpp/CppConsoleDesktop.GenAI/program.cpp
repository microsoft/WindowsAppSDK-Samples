// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <csignal>
#include <string>

#include "timing.h"
#include <exception>
#include <iostream>
#include <ostream>

#include <winrt/Windows.Foundation.h>
#include <algorithm>
#include <winrt/base.h>

#include <winrt/Microsoft.Windows.AI.MachineLearning.h>

#include "ort_genai.h"

// C++ API Example for Model Question-Answering
// This example demonstrates how to use the C++ API of the ONNX Runtime GenAI library
// to perform model question-answering tasks. It includes functionalities to create a model,
// tokenizer, and generator, and to handle user input for generating responses based on prompts.

OgaGenerator* g_generator = nullptr;

void TerminateGeneration(int)
{
    if (g_generator == nullptr)
    {
        return;
    }

    g_generator->SetRuntimeOption("terminate_session", "1");
}

void InitializeProviders(bool allowDownload)
{
    std::cout << "Getting available providers..." << std::endl;
    auto catalog = winrt::Microsoft::Windows::AI::MachineLearning::ExecutionProviderCatalog::GetDefault();
    auto providers = catalog.FindAllProviders();
    for (const auto& provider : providers)
    {
        std::wcout << L"Provider: " << provider.Name().c_str() << std::endl;
        try
        {
            auto readyState = provider.ReadyState();
            std::wcout << L"  Ready state: " << static_cast<int>(readyState) << std::endl;

            // Only call EnsureReadyAsync if we allow downloads or if the provider is already ready
            if (allowDownload || readyState != winrt::Microsoft::Windows::AI::MachineLearning::ExecutionProviderReadyState::NotPresent)
            {
                std::wcout << L"  EnsureReadyAsync" << std::endl;
                provider.EnsureReadyAsync().get();
            }

            provider.TryRegister();
        }
        catch (...)
        {
            // Continue if provider fails to initialize
        }
    }
}

void CXX_API(const char* model_path)
{
    std::cout << "Creating config..." << std::endl;
    auto config = OgaConfig::Create(model_path);

    std::cout << "Creating model..." << std::endl;
    auto model = OgaModel::Create(*config);

    std::cout << "Creating tokenizer..." << std::endl;
    auto tokenizer = OgaTokenizer::Create(*model);
    auto tokenizer_stream = OgaTokenizerStream::Create(*tokenizer);

    while (true)
    {
        std::string text;
        std::cout << "Prompt: (Use quit() to exit) Or (To terminate current output generation, press Ctrl+C)" << std::endl;
        // Clear Any cin error flags because of SIGINT
        std::cin.clear();
        std::getline(std::cin, text);

        if (text.empty())
        {
            std::cout << "Empty input. Please enter a valid prompt." << std::endl;
            continue; // Skip to the next iteration if input is empty
        }
        else if (text == "quit()")
        {
            break; // Exit the loop
        }

        signal(SIGINT, TerminateGeneration);

        const std::string messages = R"(
      [
        {
          "role": "system",
          "content": "You are a helpful AI assistant."
        },
        {
          "role": "user",
          "content": ")" + text + R"("
        }
      ]
    )";
        const std::string prompt = std::string(tokenizer->ApplyChatTemplate("", messages.c_str(), "", true));

        bool is_first_token = true;
        Timing timing;
        timing.RecordStartTimestamp();

        auto sequences = OgaSequences::Create();
        tokenizer->Encode(prompt.c_str(), *sequences);

        std::cout << "Generating response..." << std::endl;

        auto params = OgaGeneratorParams::Create(*model);
        params->SetSearchOption("max_length", 1024);
        auto generator = OgaGenerator::Create(*model, *params);
        g_generator = generator.get(); // Store the current generator for termination
        generator->AppendTokenSequences(*sequences);

        try
        {
            while (!generator->IsDone())
            {
                generator->GenerateNextToken();

                if (is_first_token)
                {
                    timing.RecordFirstTokenTimestamp();
                    is_first_token = false;
                }

                const auto num_tokens = generator->GetSequenceCount(0);
                const auto new_token = generator->GetSequenceData(0)[num_tokens - 1];
                std::cout << tokenizer_stream->Decode(new_token) << std::flush;
            }
        }
        catch (const std::exception& e)
        {
            std::cout << "\n\033[31mTerminating generation: " << e.what() << "\033[0m" << std::endl;
        }

        timing.RecordEndTimestamp();
        const int prompt_tokens_length = static_cast<int>(sequences->SequenceCount(0));
        const int new_tokens_length = static_cast<int>(generator->GetSequenceCount(0) - prompt_tokens_length);
        timing.Log(prompt_tokens_length, new_tokens_length);

        for (int i = 0; i < 3; ++i)
            std::cout << std::endl;

        g_generator = nullptr; // Clear the generator after use
    }
}

static void print_usage(int /*argc*/, char** argv)
{
    std::cerr << "usage: " << argv[0] << " <model_path> <execution_provider>" << std::endl;
    std::cerr << "  model_path: [required] Path to the folder containing onnx models, genai_config.json, etc." << std::endl;
    std::cerr << "  execution_provider: [optional] Force use of a particular execution provider (e.g. \"cpu\")" << std::endl;
    std::cerr << "                      If not specified, EP / provider options specified in genai_config.json will be used."
              << std::endl;
}

bool parse_args(int argc, char** argv, std::string& model_path)
{
    if (argc < 2)
    {
        print_usage(argc, argv);
        return false;
    }

    model_path = argv[1];

    return true;
}

int main(int argc, char** argv)
{
    bool allowDownload = true;
    InitializeProviders(allowDownload);

    std::string model_path;
    if (!parse_args(argc, argv, model_path))
    {
        return -1;
    }

    std::cout << "-------------------------" << std::endl;
    std::cout << "Hello, ORT GenAI Model-QA!" << std::endl;
    std::cout << "-------------------------" << std::endl;

    std::cout << "C++ API" << std::endl;
    try
    {
        // Responsible for cleaning up the library during shutdown
        OgaHandle handle;
        CXX_API(model_path.c_str());
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
