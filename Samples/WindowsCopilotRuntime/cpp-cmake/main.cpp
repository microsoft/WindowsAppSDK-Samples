#include <Windows.h>
#include <Unknwn.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Microsoft.Windows.AI.Generative.h>
#include <include/WindowsAppSDK-VersionInfo.h>
#include <include/MddBootstrap.h>

int wmain(int argc, wchar_t* argv[])
{
    // Initialize the Windows Runtime
    winrt::init_apartment();
    std::wstring prompt = L"You are a clever storyteller. I want to hear a story about a dragon who says: ";

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
        winrt::Microsoft::Windows::AI::Generative::LanguageModel::MakeAvailableAsync().get();
    }

    // Try again
    if (!winrt::Microsoft::Windows::AI::Generative::LanguageModel::IsAvailable())
    {
        wprintf(L"Language model is still not available.\n");
        return 1;
    }

    // Instantiate the text completion generator
    auto languageModel = winrt::Microsoft::Windows::AI::Generative::LanguageModel::CreateAsync().get();
    auto options = winrt::Microsoft::Windows::AI::Generative::LanguageModelOptions();
    options.Skill(winrt::Microsoft::Windows::AI::Generative::LanguageModelSkill::General);
    options.Temp(0.7f);
    auto response = languageModel.GenerateResponseAsync(options, prompt).get();
    wprintf(L"Status: %d\n", static_cast<int>(response.Status()));
    wprintf(L"Response: %s\n", response.Response().c_str());

    return 0;
}
