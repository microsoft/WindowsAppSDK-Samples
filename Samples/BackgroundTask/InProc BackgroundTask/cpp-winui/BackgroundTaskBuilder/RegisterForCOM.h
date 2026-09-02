// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once
#include "pch.h"
#include "BackgroundTask.h"

namespace winrt::BackgroundTaskBuilder
{
    class RegisterForCom
    {
        DWORD ComRegistrationToken = 0;
    public:
        ~RegisterForCom();
        winrt::hresult RegisterBackgroundTaskFactory();
    };
}

