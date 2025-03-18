#include <Windows.h>
#include <Unknwn.h>
#include <wil/cppwinrt.h>
#include <wil/stl.h>
#include <wil/resource.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Microsoft.Windows.AI.Generative.h>
#include <winrt/Microsoft.Windows.Management.Deployment.h>
#include <include/WindowsAppSDK-VersionInfo.h>
#include <include/MddBootstrap.h>

int wmain(int argc, wchar_t* argv[])
{
    // Initialize the Windows Runtime
    winrt::init_apartment();
    std::wstring prompt = L"You are a clever storyteller. You write evocative one-paragraph stories that a 4th grader would love. Tell a story about a dragon who says: ";

    auto bootstrapper = ::Microsoft::Windows::ApplicationModel::DynamicDependency::Bootstrap::InitializeNoThrow();
    if (FAILED(bootstrapper))
    {
        wprintf(L"Failed to initialize the Windows App SDK bootstrapper, 0x%08lx.\n", bootstrapper);
        return 1;
    }

    if (argc < 2)
    {
        wprintf(L"Usage: %s <prompt>\n", argv[0]);
        wprintf(L"Using a dummy prompt...\n");
        prompt += L"Where are my treasures?";
    }
    else
    {
        prompt += argv[1];
    }

    // Create an instance of the LanguageModel
    if (!winrt::Microsoft::Windows::AI::Generative::LanguageModel::IsAvailable())
    {
        wprintf(L"Fetching language model...\n");
        auto available = winrt::Microsoft::Windows::AI::Generative::LanguageModel::MakeAvailableAsync().get();
        if (available.Status() == winrt::Microsoft::Windows::Management::Deployment::PackageDeploymentStatus::CompletedSuccess)
        {
            wprintf(L"Language model is available.\n");
        }
        else
        {
            wprintf(L"Failed to make language model available, status: %d\n (ext error 0x%08lx) - %s\n", static_cast<int>(available.Status()), available.ExtendedError().value, available.ErrorText().c_str());
            return 1;
        }
    }

    // Instantiate the text completion generator
    auto languageModel = winrt::Microsoft::Windows::AI::Generative::LanguageModel::CreateAsync().get();
    auto options = winrt::Microsoft::Windows::AI::Generative::LanguageModelOptions();
    options.Skill(winrt::Microsoft::Windows::AI::Generative::LanguageModelSkill::General);
    options.Temp(0.7f);

    // Collect the output periodically
    auto responseWait = languageModel.GenerateResponseWithProgressAsync(prompt);
    responseWait.Progress([](auto const& sender, auto const& progress)
        {
            wprintf(L"%s", progress.c_str());
        });

    responseWait.get();

    // Run for at most 15 seconds
    if (responseWait.Status() == winrt::Windows::Foundation::AsyncStatus::Completed)
    {
        wprintf(L"Final Response: %s\n", responseWait.GetResults().Response().c_str());
    }
    else
    {
        wprintf(L"Didn't complete.\n");
    }

    return 0;
}
