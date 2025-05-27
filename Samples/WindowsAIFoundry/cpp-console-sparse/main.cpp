// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include <Windows.h>
#include <appmodel.h>
#include <Unknwn.h>
#include <libloaderapi2.h>
#include <wil/cppwinrt.h>
#include <wil/stl.h>
#include <wil/resource.h>
#include <vector>
#include <span>
#include <iostream>
#include <format>
#include <variant>
#include <winrt/Windows.Foundation.h>
#include <winrt/Microsoft.Windows.AI.Text.h>
#include <winrt/Microsoft.Windows.Management.Deployment.h>
#include <winrt/Microsoft.Windows.Workloads.h>
#include "formatters.h"

void ProcessArgs(std::vector<std::wstring_view> const& args, std::wstring& prompt, bool& useProgress);

int wmain(int argc, wchar_t* argv[])
try
{
    winrt::init_apartment();
    std::wstring instructions =
        L"You are a clever storyteller. You write engaging one-paragraph stories that grab the imagination of an 8th grader. Tell a story about a dragon who says: ";

    std::wstring prompt;
    bool useProgress = false;
    ProcessArgs({argv + 1, argv + argc}, prompt, useProgress);

    if (prompt.empty())
    {
        prompt = L"Where did my treasures go?";
    }

    // Get the language model ready for use. This may involve downloading the model and supporting
    // components. You might want to show the user a progress indicator while that's happening.
    //
    // This sample intentionally blocks until the model is available.
    auto readyState = winrt::Microsoft::Windows::AI::Text::LanguageModel::EnsureReadyAsync().get();
    if (readyState.Status() != winrt::Microsoft::Windows::AI::AIFeatureReadyResultState::Success)
    {
        std::cout << "Language model not available yet, status: " << static_cast<int>(readyState.Status()) << std::endl;
        throw std::runtime_error(std::format(
            "Language model cannot be made available, status {} error {} (ext error {}) - {}\n",
            static_cast<int>(readyState.Status()),
            static_cast<int>(readyState.Error()),
            readyState.ExtendedError().value,
            winrt::to_string(readyState.ErrorDisplayText())));
    }
    std::cout << "Language model is available." << std::endl;

    // Create an instance of the language model to use in this app.  Creating can also take time as
    // the model is loaded into the system. Consider showing a progress indicator while CreateAsync
    // is running. This sample intentionally blocks until the model is created.
    //
    // Your app can reuse instances of the LanguageModel once created.
    auto languageModel = winrt::Microsoft::Windows::AI::Text::LanguageModel::CreateAsync().get();
    auto options = winrt::Microsoft::Windows::AI::Text::LanguageModelOptions();
    options.TopK(15);
    options.Temperature(0.9f);

    // Request a response from the language model. The model will generate a response based on the
    // story prompt above.
    std::cout << "Generating response..." << std::endl << std::flush;
    if (useProgress)
    {
        // Your app can show incremental progress updates as the model is generating a response. As
        // with other generative language models, intermediate outputs may be changed or discarded
        // while the model is running. Your app should use the final Response string in its work,
        // but showing incremental progress is a great way to show the user something on the way.
        auto responseWait = languageModel.GenerateResponseAsync(instructions + prompt, options);
        responseWait.Progress([](auto const& sender, auto const& progress) {
            wprintf(L"%s", progress.c_str());
        });

        auto response = responseWait.get();
        std::wcout << std::format(L"Response: {}\n(status {})", response.Text(), static_cast<unsigned int>(response.Status()))
                   << std::endl;
    }
    else
    {
        auto response = languageModel.GenerateResponseAsync(instructions + prompt, options).get();
        std::wcout << std::format(L"Response: {}\n(status {})", response.Text(), static_cast<unsigned int>(response.Status()))
                   << std::endl;
    }

    return 0;
}
catch (std::exception const& e)
{
    std::cerr << std::format("Exception: {}", e.what()) << std::endl;
    return -1;
}
catch (...)
{
    auto ex = wil::ResultFromCaughtException();
    std::cerr << std::format("Exception: 0x{:08x}", static_cast<uint32_t>(ex)) << std::endl;
    return ex;
}

#if defined(_M_ARM64)
const auto g_packageArchitecture = PackageDependencyProcessorArchitectures_Arm64;
#elif defined(_M_X64)
const auto g_packageArchitecture = PackageDependencyProcessorArchitectures_X64;
#endif

void ProcessArgs(std::vector<std::wstring_view> const& args, std::wstring& prompt, bool& useProgress)
{
    for (auto& arg : args)
    {
        if (arg == L"--progress")
        {
            useProgress = true;
        }
        else if ((arg == L"--help") || (arg == L"-h"))
        {
            std::wcout << std::format(L"Usage {} [--progress] <prompt text>\n", args[0]) << std::endl;
            std::wcout << L"  --progress: Show progress updates." << std::endl;
            std::wcout << L"  --help, -h: Show this help message." << std::endl;
            std::wcout << L"  <prompt text>: The prompt to use for the language model." << std::endl;
            exit(0);
        }
        else
        {
            prompt.append(L" ").append(arg);
        }
    }
}
