// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.

//
// Application Implementation
//
// Implementation of main application logic for the Windows ML ABI sample.
//

#include "Application.h"
#include "ConsoleUI.h"
#include <roapi.h>
#include <iostream>
#include <stdexcept>

namespace CppAbiEPEnumerationSample
{

    void Application::Run()
    {
        // Initialize Windows Runtime outside try/catch for proper cleanup
        HRESULT hr = RoInitialize(RO_INIT_MULTITHREADED);
        if (FAILED(hr))
        {
            std::wcout << L"Failed to initialize Windows Runtime. HRESULT: 0x"
                       << std::hex << hr << std::endl;
            return;
        }

        try
        {
            RunInteractiveSession();
        }
        catch (const std::exception& e)
        {
            std::wcout << L"Error: " << std::wstring(e.what(), e.what() + strlen(e.what())) << std::endl;
        }

        // Uninitialize Windows Runtime after all COM objects are destroyed
        RoUninitialize();
    }

    void Application::RunInteractiveSession()
    {
        ConsoleUI::PrintHeader();

        // Create catalog and find providers
        ExecutionProviderCatalog catalog;
        auto providers = catalog.FindAllProviders();

        if (providers.empty())
        {
            std::wcout << L"No execution providers found." << std::endl;
            return;
        }

        while (true)
        {
            ConsoleUI::PrintProviders(providers);

            int selection = ConsoleUI::GetUserSelection(static_cast<int>(providers.size()));

            if (selection == 0)
            {
                break;
            }

            if (selection < 1 || selection > static_cast<int>(providers.size()))
            {
                std::wcout << L"Invalid selection." << std::endl;
                continue;
            }

            const auto& selectedProvider = providers[selection - 1];
            ProcessProviderSelection(selectedProvider);

            ConsoleUI::WaitForKeypress();
        }
    }

    void Application::ProcessProviderSelection(const ExecutionProvider& provider)
    {
        std::wcout << L"Selected: " << provider.GetName() << std::endl;

        auto currentState = provider.GetReadyState();
        if (currentState == ABI::Microsoft::Windows::AI::MachineLearning::ExecutionProviderReadyState_Ready)
        {
            std::wcout << L"Provider is already ready!" << std::endl;
            TryRegisterProvider(provider);
        }
        else
        {
            std::wcout << L"Provider current state: " << provider.GetReadyStateString(currentState) << std::endl;

            if (currentState == ABI::Microsoft::Windows::AI::MachineLearning::ExecutionProviderReadyState_NotPresent ||
                currentState == ABI::Microsoft::Windows::AI::MachineLearning::ExecutionProviderReadyState_NotReady)
            {
                PrepareProvider(provider);
            }
            else
            {
                // Try to register for other states
                TryRegisterProvider(provider);
            }
        }
    }

    void Application::PrepareProvider(const ExecutionProvider& provider)
    {
        std::wcout << L"Calling EnsureReadyAsync to prepare the provider..." << std::endl;

        bool ensureReadySuccess = provider.EnsureReadyAsync();

        if (ensureReadySuccess)
        {
            // Check the state again after EnsureReady
            auto newState = provider.GetReadyState();
            std::wcout << L"Provider state after EnsureReady: "
                       << provider.GetReadyStateString(newState) << std::endl;

            if (newState == ABI::Microsoft::Windows::AI::MachineLearning::ExecutionProviderReadyState_Ready)
            {
                TryRegisterProvider(provider);
            }
            else
            {
                std::wcout << L"Provider is not ready even after EnsureReadyAsync." << std::endl;
            }
        }
        else
        {
            std::wcout << L"EnsureReadyAsync failed." << std::endl;
        }
    }

    void Application::TryRegisterProvider(const ExecutionProvider& provider)
    {
        if (provider.TryRegister())
        {
            std::wcout << L"Provider registered successfully!" << std::endl;
        }
        else
        {
            std::wcout << L"Provider registration failed." << std::endl;
        }
    }

} // namespace CppAbiEPEnumerationSample