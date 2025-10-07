// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.
#pragma once

//
// Async Handlers Header
//
// Event-driven async operation handlers for Windows ML ABI interfaces.
// Provides progress reporting and completion notification with HANDLE-based synchronization.
//

#include <wrl/implements.h>
#include <functional>
#include "Microsoft.Windows.AI.MachineLearning.h"
#include "Windows.Foundation.h"

namespace CppAbiEPEnumerationSample
{

    /// <summary>
    /// Progress handler for async operations with progress reporting
    /// </summary>
    class ProgressHandler : public Microsoft::WRL::RuntimeClass<
        Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>,
        __FIAsyncOperationProgressHandler_2_Microsoft__CWindows__CAI__CMachineLearning__CExecutionProviderReadyResult_double>
    {
    public:
        ProgressHandler(std::function<void(double)> callback);

        // IAsyncOperationProgressHandler implementation
        IFACEMETHOD(Invoke)(
            __FIAsyncOperationWithProgress_2_Microsoft__CWindows__CAI__CMachineLearning__CExecutionProviderReadyResult_double* asyncInfo,
            double progressInfo) override;

    private:
        std::function<void(double)> m_callback;
    };

    /// <summary>
    /// Completion handler for async operations with event synchronization
    /// </summary>
    class CompletionHandler : public Microsoft::WRL::RuntimeClass<
        Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>,
        __FIAsyncOperationWithProgressCompletedHandler_2_Microsoft__CWindows__CAI__CMachineLearning__CExecutionProviderReadyResult_double>
    {
    public:
        CompletionHandler(HANDLE completionEvent);

        // IAsyncOperationWithProgressCompletedHandler implementation
        IFACEMETHOD(Invoke)(
            __FIAsyncOperationWithProgress_2_Microsoft__CWindows__CAI__CMachineLearning__CExecutionProviderReadyResult_double* asyncInfo,
            ABI::Windows::Foundation::AsyncStatus asyncStatus) override;

        ABI::Windows::Foundation::AsyncStatus GetStatus() const;
        HRESULT GetErrorCode() const;

    private:
        const HANDLE m_completionEvent;
        ABI::Windows::Foundation::AsyncStatus m_status;
        HRESULT m_errorCode;
    };

} // namespace CppAbiEPEnumerationSample