// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.

//
// Execution Provider Implementation
//
// Implementation of Windows ML execution provider ABI wrapper.
//

#include "ExecutionProvider.h"
#include "AbiUtils.h"
#include "AsyncHandlers.h"
#include <wrl/module.h>
#include <iostream>
#include <iomanip>

namespace CppAbiEPEnumerationSample
{

    ExecutionProvider::ExecutionProvider(Microsoft::WRL::ComPtr<ABI::Microsoft::Windows::AI::MachineLearning::IExecutionProvider> provider)
        : m_provider(provider)
    {
    }

    std::wstring ExecutionProvider::GetName() const
    {
        HStringWrapper name;
        HRESULT hr = m_provider->get_Name(name.put());
        if (SUCCEEDED(hr))
        {
            return name.ToString();
        }
        return L"Unknown";
    }

    std::wstring ExecutionProvider::GetDescription() const
    {
        HStringWrapper path;
        HRESULT hr = m_provider->get_LibraryPath(path.put());
        if (SUCCEEDED(hr))
        {
            std::wstring libraryPath = path.ToString();
            if (!libraryPath.empty())
            {
                return L"Library: " + libraryPath;
            }
        }
        return L"(please ensure ready)";
    }

    ABI::Microsoft::Windows::AI::MachineLearning::ExecutionProviderReadyState ExecutionProvider::GetReadyState() const
    {
        ABI::Microsoft::Windows::AI::MachineLearning::ExecutionProviderReadyState state;
        HRESULT hr = m_provider->get_ReadyState(&state);
        if (SUCCEEDED(hr))
        {
            return state;
        }
        return ABI::Microsoft::Windows::AI::MachineLearning::ExecutionProviderReadyState_NotPresent;
    }

    std::wstring ExecutionProvider::GetReadyStateString(ABI::Microsoft::Windows::AI::MachineLearning::ExecutionProviderReadyState state) const
    {
        switch (state)
        {
            case ABI::Microsoft::Windows::AI::MachineLearning::ExecutionProviderReadyState_NotPresent: return L"Not Present";
            case ABI::Microsoft::Windows::AI::MachineLearning::ExecutionProviderReadyState_NotReady: return L"Not Ready";
            case ABI::Microsoft::Windows::AI::MachineLearning::ExecutionProviderReadyState_Ready: return L"Ready";
            default: return L"Unknown";
        }
    }

    bool ExecutionProvider::TryRegister() const
    {
        boolean result = false;
        HRESULT hr = m_provider->TryRegister(&result);
        return SUCCEEDED(hr) && result;
    }

    bool ExecutionProvider::EnsureReadyAsync() const
    {
        // Create an event for synchronization
        HANDLE completionEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
        if (completionEvent == nullptr)
        {
            std::wcout << L"Failed to create synchronization event" << std::endl;
            return false;
        }

        // Get the async operation
        Microsoft::WRL::ComPtr<__FIAsyncOperationWithProgress_2_Microsoft__CWindows__CAI__CMachineLearning__CExecutionProviderReadyResult_double> operation;
        HRESULT hr = m_provider->EnsureReadyAsync(&operation);

        if (FAILED(hr))
        {
            std::wcout << L"Failed to start EnsureReadyAsync operation. HRESULT: 0x" << std::hex << hr << std::endl;
            ::CloseHandle(completionEvent);
            return false;
        }

        // Create and attach progress handler
        auto progressHandler = Microsoft::WRL::Make<ProgressHandler>([](double progress) {
            std::wcout << L"Progress: " << std::fixed << std::setprecision(1) << (progress * 100.0) << L"%" << std::endl;
        });

        hr = operation->put_Progress(progressHandler.Get());
        if (FAILED(hr))
        {
            std::wcout << L"Warning: Failed to attach progress handler. HRESULT: 0x" << std::hex << hr << std::endl;
        }

        // Create and attach completion handler
        auto completionHandler = Microsoft::WRL::Make<CompletionHandler>(completionEvent);
        if (completionHandler == nullptr)
        {
            std::wcout << L"Failed to create completion handler" << std::endl;
            ::CloseHandle(completionEvent);
            return false;
        }

        hr = operation->put_Completed(completionHandler.Get());
        if (FAILED(hr))
        {
            std::wcout << L"Failed to attach completion handler. HRESULT: 0x" << std::hex << hr << std::endl;
            ::CloseHandle(completionEvent);
            return false;
        }

        // Wait for the operation to complete
        ::WaitForSingleObject(completionEvent, INFINITE);

        // Check the result
        auto status = completionHandler->GetStatus();
        bool success = (status == ABI::Windows::Foundation::AsyncStatus::Completed);

        if (!success)
        {
            if (status == ABI::Windows::Foundation::AsyncStatus::Error)
            {
                HRESULT errorCode = completionHandler->GetErrorCode();
                std::wcout << L"EnsureReadyAsync failed with error: 0x" << std::hex << errorCode << std::endl;
            }
            else if (status == ABI::Windows::Foundation::AsyncStatus::Canceled)
            {
                std::wcout << L"EnsureReadyAsync was canceled" << std::endl;
            }
        }

        // Cleanup
        ::CloseHandle(completionEvent);
        return success;
    }

} // namespace CppAbiEPEnumerationSample
