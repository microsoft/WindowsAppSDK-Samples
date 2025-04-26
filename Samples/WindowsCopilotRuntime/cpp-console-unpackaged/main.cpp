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
#include <winrt/Microsoft.Windows.AI.Generative.h>
#include <winrt/Microsoft.Windows.Management.Deployment.h>
#include <winrt/Microsoft.Windows.Workloads.h>
#include <include/WindowsAppSDK-VersionInfo.h>
#include <include/MddBootstrap.h>
#include "formatters.h"

using unique_dependency = wil::unique_any<PACKAGEDEPENDENCY_CONTEXT, decltype(RemovePackageDependency), RemovePackageDependency>;
using wasdk_usage =
    std::variant<unique_dependency, ::Microsoft::Windows::ApplicationModel::DynamicDependency::Bootstrap::unique_mddbootstrapshutdown>;
wasdk_usage AcquireWinAppSDK(bool useManualBootstrap);
void ProcessArgs(std::vector<std::wstring_view> const& args, std::wstring& prompt, bool& manualBootstrap, bool& useProgress);

int wmain(int argc, wchar_t* argv[])
try
{
    winrt::init_apartment();
    std::wstring instructions =
        L"You are a clever storyteller. You write engaging one-paragraph stories that grab the imagination of an 8th grader. Tell a story about a dragon who says: ";

    std::wstring prompt;
    bool manualBootstrap = false;
    bool useProgress = false;
    ProcessArgs({argv + 1, argv + argc}, prompt, manualBootstrap, useProgress);

    // Load the Windows App SDK for this process
    auto dependency = AcquireWinAppSDK(manualBootstrap);

    if (prompt.empty())
    {
        prompt = L"Where did my treasures go?";
    }

    // Get the language model ready for use. This may involve downloading the model and supporting
    // components. You might want to show the user a progress indicator while that's happening.
    //
    // This sample intentionally blocks until the model is available.
    auto readyState = winrt::Microsoft::Windows::AI::Generative::LanguageModel::EnsureReadyAsync().get();
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
    auto languageModel = winrt::Microsoft::Windows::AI::Generative::LanguageModel::CreateAsync().get();
    auto options = winrt::Microsoft::Windows::AI::Generative::LanguageModelOptions();
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

wasdk_usage AcquireWinAppSDK(bool useManualMethod)
{
    // See https://learn.microsoft.com/windows/apps/windows-app-sdk/use-windows-app-sdk-run-time
    // for more on using the Windows App SDK in a framework-dependent unpackaged application.
    //
    // Windows App SDK's framework package must be pulled into your process' package graph
    // before you can use it.  There's two methods available - one is the "easy mode", calling
    // into the Bootstrapper C++ library, and one is the "manual mode" where your app creates
    // and manages a package dependency.

    if (useManualMethod)
    {
        // The package dependency APIs are new as of Windows 11 (10.0.22000). If your app is
        // running on older systems, you'll need to use the Bootstrapper module below, which
        // uses other mechanisms to find and connect to the runtime.
        HMODULE selfModule;
        GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, reinterpret_cast<LPCSTR>(AcquireWinAppSDK), &selfModule);
        auto hasTryCreate = ::QueryOptionalDelayLoadedAPI(selfModule, "*", "TryCreatePackageDependency", 0);
        auto hasAddPackage = ::QueryOptionalDelayLoadedAPI(selfModule, "*", "AddPackageDependency", 0);
        if (!hasTryCreate || !hasAddPackage)
        {
            throw std::runtime_error("This version of Windows does not support manual load of the Windows App Runtime");
        }

        // The constants in Microsoft::WindowsAppSDK::Runtime are defined in WindowsAppSDK-VersionInfo.h
        // and specify the Windows App SDK used when building your app.
        //
        // Adding a package dependency requires the family name, desired version, and processor
        // architecture of the package to load.
        //
        // Using the "Process" lifetime means the dependency is only used while this process
        // is running. When this process terminates, the dependency is removed.
        PACKAGE_VERSION myVersion;
        myVersion.Major = ::Microsoft::WindowsAppSDK::Runtime::Version::Major;
        myVersion.Minor = ::Microsoft::WindowsAppSDK::Runtime::Version::Minor;
        myVersion.Build = ::Microsoft::WindowsAppSDK::Runtime::Version::Build;
        myVersion.Revision = ::Microsoft::WindowsAppSDK::Runtime::Version::Revision;
        wil::unique_process_heap_string dependencyId;
        THROW_IF_FAILED_MSG(
            ::TryCreatePackageDependency(
                nullptr,
                ::Microsoft::WindowsAppSDK::Runtime::Packages::Framework::PackageFamilyName,
                myVersion,
                g_packageArchitecture,
                PackageDependencyLifetimeKind_Process,
                nullptr,
                CreatePackageDependencyOptions_None,
                &dependencyId),
            "Couldn't create a package dependency");

        wil::unique_process_heap_string packageFullName;
        unique_dependency dependencyContext;
        THROW_IF_FAILED_MSG(
            ::AddPackageDependency(
                dependencyId.get(), 1, AddPackageDependencyOptions_PrependIfRankCollision, &dependencyContext, &packageFullName),
            "Couldn't add package dependency");

        return dependencyContext;
    }
    else
    {
        // Use the provided Bootstrap method, which sets the package dependency for this process.
        //
        // See https://learn.microsoft.com/windows/apps/api-reference/bootstrapper-cpp-api/microsoft.windows.applicationmodel.dynamicdependency.bootstrap/microsoft.windows.applicationmodel.dynamicdependency.bootstrap
        // for more details. If your app does not use C++ exceptions, InitializeNoThrow() returns
        // an HRESULT error you can handle. This mode asks the user to install the Windows App SDK
        // if it's not present.
        auto options = ::Microsoft::Windows::ApplicationModel::DynamicDependency::Bootstrap::InitializeOptions::OnNoMatch_ShowUI;
        auto result = ::Microsoft::Windows::ApplicationModel::DynamicDependency::Bootstrap::InitializeNoThrow(
            Microsoft::WindowsAppSDK::Release::MajorMinor,
            Microsoft::WindowsAppSDK::Release::VersionTag,
            Microsoft::WindowsAppSDK::Runtime::Version::UInt64,
            options);
        if (FAILED(result))
        {
            std::wcerr << std::format(
                L"Couldn't find Windows App SDK {}.{}{}, error: 0x{:08x}",
                Microsoft::WindowsAppSDK::Release::Major,
                Microsoft::WindowsAppSDK::Release::Minor,
                Microsoft::WindowsAppSDK::Release::FormattedVersionTag,
                              static_cast<uint32_t>(result))
                       << std::endl;

            throw winrt::hresult_error(result);
        }

        return Microsoft::Windows::ApplicationModel::DynamicDependency::Bootstrap::unique_mddbootstrapshutdown{
            reinterpret_cast<Microsoft::Windows::ApplicationModel::DynamicDependency::Bootstrap::details::mddbootstrapshutdown_t*>(1)};
    }
}

void ProcessArgs(std::vector<std::wstring_view> const& args, std::wstring& prompt, bool& manualBootstrap, bool& useProgress)
{
    for (auto& arg : args)
    {
        if (arg == L"--manual-bootstrap")
        {
            manualBootstrap = true;
        }
        else if (arg == L"--progress")
        {
            useProgress = true;
        }
        else if ((arg == L"--help") || (arg == L"-h"))
        {
            std::wcout << std::format(L"Usage {} [--manual-bootstrap] [--progress] <prompt text>\n", args[0]) << std::endl;
            std::wcout << L"  --manual-bootstrap: Use manual bootstrap instead of the WinAppSDK bootstrapper." << std::endl;
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
