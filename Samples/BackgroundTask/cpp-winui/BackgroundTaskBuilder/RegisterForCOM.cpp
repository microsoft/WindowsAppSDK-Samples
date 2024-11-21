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
        try
        {
            handle _taskFactoryCompletionEvent{ CreateEvent(nullptr, false, false, nullptr) };
            com_ptr<IClassFactory> taskFactory = make<BackgroundTaskFactory>();

            check_hresult(CoRegisterClassObject(classId,
                taskFactory.detach(),
                CLSCTX_LOCAL_SERVER,
                REGCLS_MULTIPLEUSE,
                &ComRegistrationToken));

            OutputDebugString(L"COM Registraton done");
            hr = S_OK;
        }
        catch (...)
        {
            hr = to_hresult();
        }

        return hr;
    }
}
