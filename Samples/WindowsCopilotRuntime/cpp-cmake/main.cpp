#include <Windows.h>
#include <appmodel.h>
#include <Unknwn.h>
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

using unique_dependency = wil::unique_any<PACKAGEDEPENDENCY_CONTEXT, decltype(RemovePackageDependency), RemovePackageDependency>;
using wasdk_usage = std::variant<unique_dependency, ::Microsoft::Windows::ApplicationModel::DynamicDependency::Bootstrap::unique_mddbootstrapshutdown>;
wasdk_usage AcquireWinAppSDK(bool useManualBootstrap);
void ProcessArgs(std::vector<std::wstring_view> const& args, std::wstring& prompt, bool& manualBootstrap, bool& useProgress);

namespace std
{
    template<typename CharT> struct formatter<winrt::hstring, CharT>
    {
        template<class ParseContext>
        constexpr auto parse(ParseContext& ctx)
        {
            auto it = ctx.begin();
            if (it != ctx.end() && *it != '}')
            {
                throw std::runtime_error("Invalid format specifier");
            }

            return it;
        }

        template<class Context>
        constexpr auto format(winrt::hstring const& str, Context& ctx) const
        {
            return std::ranges::copy(str.c_str(), str.c_str() + str.size(), ctx.out());
        }
    };
}

int wmain(int argc, wchar_t* argv[]) try
{
    winrt::init_apartment();
    std::wstring instructions = L"You are a clever storyteller. You write engaging one-paragraph stories that grab the imagination of an 8th grader. Tell a story about a dragon who says: ";
    
    std::wstring prompt;
    bool manualBootstrap = false;
    bool useProgress = false;
    ProcessArgs({ argv, argv + argc }, prompt, manualBootstrap, useProgress);

    // Load the Windows App SDK for this process
    auto dependency = AcquireWinAppSDK(manualBootstrap);

    if (prompt.empty())
    {
        prompt = L"Where did my treasures go?";
    }

    // Check to see if the langauge model is available on this machine. If it's not, the workflow
    // system can download it on demand. The very first download of the model takes more time than
    // subsequent downloads.
    //
    // An interactive app could show a progress dialog while MakeAvailableAsync() is running. This
    // sample intentionally blocks until the model is available.
    if (!winrt::Microsoft::Windows::AI::Generative::LanguageModel::IsAvailable())
    {
        std::cout << "Fetching language model..." << std::endl << std::flush;
        auto available = winrt::Microsoft::Windows::AI::Generative::LanguageModel::MakeAvailableAsync().get();
        if (available.Status() != winrt::Microsoft::Windows::Management::Deployment::PackageDeploymentStatus::CompletedSuccess)
        {
            throw std::runtime_error(std::format("Failed to make language model available, status: {} (ext error {}) - {}\n",
                static_cast<int>(available.Status()), available.ExtendedError().value, winrt::to_string(available.ErrorText())));
        }

        std::cout << "Language model is available." << std::endl;
    }

    // Create an instance of the language model to use in this app.  Creating can also take time as
    // the model is loaded into the system. Consider showing a progress indicator while CreateAsync
    // is running. This sample intentionally blocks until the model is created.
    //
    // Your app can reuse instances of the LanguageModel once created.
    auto languageModel = winrt::Microsoft::Windows::AI::Generative::LanguageModel::CreateAsync().get();
    auto options = winrt::Microsoft::Windows::AI::Generative::LanguageModelOptions();
    options.Skill(winrt::Microsoft::Windows::AI::Generative::LanguageModelSkill::General);
    options.Temp(0.7f);

    // Request a response from the language model. The model will generate a response based on the
    // story prompt above.
    std::cout << "Generating response..." << std::endl << std::flush;
    if (useProgress)
    {
        // Your app can show incremental progress updates as the model is generating a response. As
        // with other generative language models, intermediate outputs may be changed or discarded
        // while the model is running. Your app should use the final Response string in its work,
        // but showing incremental progress is a great way to show the user something on the way.
        auto responseWait = languageModel.GenerateResponseWithProgressAsync(options, instructions + prompt);
        responseWait.Progress([](auto const& sender, auto const& progress)
            {
                wprintf(L"%s", progress.c_str());
            });

        auto response = responseWait.get();
        std::wcout << std::format(L"Response: {}\n(status {})", response.Response(), static_cast<unsigned int>(response.Status())) << std::endl;
    }
    else
    {
        auto response = languageModel.GenerateResponseAsync(prompt).get();
        std::wcout << std::format(L"Response: {}\n(status {})", response.Response(), static_cast<unsigned int>(response.Status())) << std::endl;
    }

    return 0;
}
catch (std::exception const& e)
{
    std::cerr << std::format("Exception: {} (0x{:08x})\n", e.what(), static_cast<unsigned int>(GetLastError())) << std::endl;
    return -1;
}
catch (...)
{
    auto ex = wil::ResultFromCaughtException();
    std::cerr << std::format("Exception: 0x{:08x}", ex) << std::endl;
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
        // The constants in Microsoft::WindowsAppSDK::Runtime are defined by a Windows App SDK
        // header file and specify the Windows App SDK version your app compiled against.
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
        THROW_IF_FAILED_MSG(::TryCreatePackageDependency(nullptr,
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
        THROW_IF_FAILED_MSG(::AddPackageDependency(
            dependencyId.get(),
            1,
            AddPackageDependencyOptions_PrependIfRankCollision,
            &dependencyContext,
            &packageFullName),
            "Couldn't add package dependency");

        return dependencyContext;
    }
    else
    {
        // Use the provided Bootstrap method, which sets the package dependency for this process.
        //
        // See https://learn.microsoft.com/windows/apps/api-reference/bootstrapper-cpp-api/microsoft.windows.applicationmodel.dynamicdependency.bootstrap/microsoft.windows.applicationmodel.dynamicdependency.bootstrap
        // for more details. If your app does not use C++ exceptions, InitializeNoThrow() returns
        // an HRESULT error you can handle.
        return ::Microsoft::Windows::ApplicationModel::DynamicDependency::Bootstrap::Initialize();
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
