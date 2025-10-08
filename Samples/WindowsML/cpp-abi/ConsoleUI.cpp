// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.

//
// Console UI Implementation
//
// Implementation of user interface utilities for the Windows ML sample application.
//

#include "ConsoleUI.h"
#include <iostream>
#include <iomanip>

namespace CppAbiEPEnumerationSample
{

    void ConsoleUI::PrintHeader()
    {
        std::wcout << L"=== Windows ML Execution Provider Sample ===" << std::endl;
        std::wcout << L"Using C++ ABI/COM interfaces" << std::endl;
        std::wcout << L"===========================================" << std::endl << std::endl;
    }

    void ConsoleUI::PrintProviders(const std::vector<ExecutionProvider>& providers)
    {
        std::wcout << L"Available Execution Providers:" << std::endl;
        for (size_t i = 0; i < providers.size(); i++)
        {
            const auto& provider = providers[i];
            auto state = provider.GetReadyState();
            auto stateStr = provider.GetReadyStateString(state);

            std::wcout << L"  " << (i + 1) << L". " << provider.GetName()
                       << L" [" << stateStr << L"]" << std::endl;
            std::wcout << L"     " << provider.GetDescription() << std::endl;
        }
        std::wcout << std::endl;
    }

    int ConsoleUI::GetUserSelection(int maxOptions)
    {
        int selection = 0;
        if (maxOptions == 1)
        {
            std::wcout << L"Select the provider (1), or 0 to exit: ";
        }
        else
        {
            std::wcout << L"Select a provider (1-" << maxOptions << L"), or 0 to exit: ";
        }
        std::wcin >> selection;
        return selection;
    }

    void ConsoleUI::PrintProgress(double progress)
    {
        const int barWidth = 50;
        int filled = static_cast<int>(progress * barWidth);

        std::wcout << L"\rProgress: [";
        for (int i = 0; i < barWidth; i++)
        {
            if (i < filled)
            {
                std::wcout << L"#";
            }
            else
            {
                std::wcout << L"-";
            }
        }
        std::wcout << L"] " << std::fixed << std::setprecision(1) << (progress * 100) << L"%";
        std::wcout.flush();
    }

    void ConsoleUI::PrintResult(bool success)
    {
        std::wcout << std::endl;
        if (success)
        {
            std::wcout << L"Provider is ready!" << std::endl;
        }
        else
        {
            std::wcout << L"Provider failed to initialize." << std::endl;
        }
    }

    void ConsoleUI::WaitForKeypress()
    {
        std::wcout << L"Press Enter to continue...";
        std::wcin.ignore();
        std::wcin.get();
    }

} // namespace CppAbiEPEnumerationSample