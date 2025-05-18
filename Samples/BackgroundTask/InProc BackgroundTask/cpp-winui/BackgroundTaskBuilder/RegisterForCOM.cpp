// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "RegisterForCOM.h"

using namespace winrt;

namespace winrt::BackgroundTaskBuilder
{
    RegisterForCom::~RegisterForCom()
    {
        if (ComRegistrationToken != 0)
        {
            CoRevokeClassObject(ComRegistrationToken);
        }
    }

    hresult RegisterForCom::RegisterBackgroundTaskFactory()
    {
        try
        {
            com_ptr<IClassFactory> taskFactory = make<BackgroundTaskFactory>();

            check_hresult(CoRegisterClassObject(__uuidof(BackgroundTask),
                taskFactory.detach(),
                CLSCTX_LOCAL_SERVER,
                REGCLS_MULTIPLEUSE,
                &ComRegistrationToken));

            OutputDebugString(L"COM Registration done");
            return S_OK;
        }
        CATCH_RETURN();
    }
}
