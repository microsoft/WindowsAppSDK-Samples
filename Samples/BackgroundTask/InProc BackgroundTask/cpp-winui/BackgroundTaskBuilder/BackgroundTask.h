// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "pch.h"
#include "MainWindow.g.h"

#define CLSID_BackgroundTask "12345678-1234-1234-1234-1234567890CD"
namespace winrt::BackgroundTaskBuilder
{
    struct __declspec(uuid(CLSID_BackgroundTask))
    BackgroundTask : implements<BackgroundTask, winrt::Windows::ApplicationModel::Background::IBackgroundTask>
    {
        void Run(_In_ winrt::Windows::ApplicationModel::Background::IBackgroundTaskInstance taskInstance);
    private:
        void OnCanceled(_In_ winrt::Windows::ApplicationModel::Background::IBackgroundTaskInstance /* taskInstance */, _In_ winrt::Windows::ApplicationModel::Background::BackgroundTaskCancellationReason /* cancelReason */);
        volatile bool m_cancelRequested = false;
        winrt::Windows::ApplicationModel::Background::BackgroundTaskDeferral m_deferral = nullptr;
        Windows::System::Threading::ThreadPoolTimer m_periodicTimer = nullptr;
        winrt::BackgroundTaskBuilder::IMainWindow m_mainWindow = nullptr;
        int m_progress{ 0 };
    };

    struct BackgroundTaskFactory : implements<BackgroundTaskFactory, IClassFactory>
    {
        HRESULT __stdcall CreateInstance(_In_opt_ IUnknown* aggregateInterface, _In_ REFIID interfaceId, _Outptr_ VOID** object) noexcept final
        {
            if (aggregateInterface != NULL) {
                return CLASS_E_NOAGGREGATION;
            }

            return make<BackgroundTask>().as(interfaceId, object);
        }

        HRESULT __stdcall LockServer(BOOL lock) noexcept final
        {
            UNREFERENCED_PARAMETER(lock);
            return S_OK;
        }
    };
}

