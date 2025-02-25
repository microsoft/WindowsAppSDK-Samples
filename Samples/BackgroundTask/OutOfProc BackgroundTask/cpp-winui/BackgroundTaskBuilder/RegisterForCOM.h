// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once
#include "pch.h"
#include "BackgroundTask.h"

namespace winrt::BackgroundTaskBuilder
{
    static handle _comServerCompletionEvent;
    class RegisterForCom
    {
        DWORD ComRegistrationToken = 0;
    public:
        ~RegisterForCom();
        winrt::hresult RegisterAndWait(winrt::guid classId);
        static void StopServer();
        static constexpr wchar_t* RegisterForComToken = L"-RegisterForBGTaskServer";
    };
}

