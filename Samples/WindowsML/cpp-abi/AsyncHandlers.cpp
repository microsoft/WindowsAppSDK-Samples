// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.

//
// Async Handlers Implementation
//
// Implementation of event-driven async operation handlers for Windows ML ABI interfaces.
//

#include "AsyncHandlers.h"
#include <iostream>

namespace CppAbiEPEnumerationSample
{

    // ProgressHandler Implementation
    ProgressHandler::ProgressHandler(std::function<void(double)> callback)
        : m_callback(std::move(callback))
    {
    }

    HRESULT ProgressHandler::Invoke(
        __FIAsyncOperationWithProgress_2_Microsoft__CWindows__CAI__CMachineLearning__CExecutionProviderReadyResult_double* /*asyncInfo*/,
        double progressInfo)
    {
        if (m_callback) {
            m_callback(progressInfo);
        }
        return S_OK;
    }

    // CompletionHandler Implementation
    CompletionHandler::CompletionHandler(HANDLE completionEvent)
        : m_completionEvent(completionEvent)
        , m_status(ABI::Windows::Foundation::AsyncStatus::Started)
        , m_errorCode(S_OK)
    {
    }

    HRESULT CompletionHandler::Invoke(
        __FIAsyncOperationWithProgress_2_Microsoft__CWindows__CAI__CMachineLearning__CExecutionProviderReadyResult_double* asyncInfo,
        ABI::Windows::Foundation::AsyncStatus asyncStatus)
    {
        m_status = asyncStatus;

        if (asyncStatus == ABI::Windows::Foundation::AsyncStatus::Error) {
            // Get error code from the async operation
            Microsoft::WRL::ComPtr<ABI::Windows::Foundation::IAsyncInfo> asyncInfoInterface;
            HRESULT hrQI = asyncInfo->QueryInterface(IID_PPV_ARGS(&asyncInfoInterface));
            if (SUCCEEDED(hrQI) && asyncInfoInterface) {
                asyncInfoInterface->get_ErrorCode(&m_errorCode);
            }
        }

        SetEvent(m_completionEvent);
        return S_OK;
    }

    ABI::Windows::Foundation::AsyncStatus CompletionHandler::GetStatus() const
    {
        return m_status;
    }

    HRESULT CompletionHandler::GetErrorCode() const
    {
        return m_errorCode;
    }

} // namespace CppAbiEPEnumerationSample