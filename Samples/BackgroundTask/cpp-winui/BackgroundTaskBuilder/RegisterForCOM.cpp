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

    hresult RegisterForCom::RegisterAndWait(guid classId)
    {
        hresult hr;
        _comServerCompletionEvent.attach(CreateEvent(nullptr, true, false, nullptr));
        try
        {
            check_bool(bool{ _comServerCompletionEvent });
            com_ptr<IClassFactory> taskFactory = make<BackgroundTaskFactory>();

            check_hresult(CoRegisterClassObject(classId,
                taskFactory.detach(),
                CLSCTX_LOCAL_SERVER,
                REGCLS_MULTIPLEUSE,
                &ComRegistrationToken));

            OutputDebugString(L"COM Registraton done");
            // Wait for the COM calls to complete
            check_hresult(WaitForSingleObject(_comServerCompletionEvent.get(), INFINITE));
            hr = S_OK;
        }
        catch (...)
        {
            hr = to_hresult();
        }

        return hr;
    }

    void RegisterForCom::StopServer()
    {
        SetEvent(_comServerCompletionEvent.get());
    }
}
