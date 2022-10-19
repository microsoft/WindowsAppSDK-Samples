// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "WidgetProvider.h"
#include <mutex>
#include <winstring.h>
#include <iostream>
#include <objidl.h>

// This GUID is the same GUID that was provided in the 
// registration of the COM Server and Class Id in the .appxmanifest.
static constexpr GUID widget_provider_clsid
{ /* 101D03A3-6FC8-4887-9B64-310A7B164319 */
    0x101d03a3, 0x6fc8, 0x4887, {0x9b, 0x64, 0x31, 0x0a, 0x7b, 0x16, 0x43, 0x19}
};

// This is implementation of a ClassFactory that will instantiate WidgetProvider
// that will interact with the Widget Service.
template <typename T>
struct SingletonClassFactory : winrt::implements<SingletonClassFactory<T>, IClassFactory>
{
    STDMETHODIMP CreateInstance(
        ::IUnknown* outer,
        GUID const& iid,
        void** result) noexcept final
    {
        *result = nullptr;

        std::unique_lock lock(mutex);

        if (outer)
        {
            return CLASS_E_NOAGGREGATION;
        }

        return winrt::make_self<T>().as(iid, result);
    }

    STDMETHODIMP LockServer(BOOL flock) noexcept final
    {
        return S_OK;
    }

private:
    //winrt::com_ptr<T> instance{ nullptr };
    std::mutex mutex;
};

#include <windows.h>

inline HMODULE GetComBaseModuleHandle()
{
    /*
    CoGetSharedServiceId @66 NONAME PRIVATE_API
    CoAddRefSharedService @67 NONAME PRIVATE_API
    CoReleaseSharedService @68 NONAME PRIVATE_API
    CoRegisterServerShutdownDelay @69 NONAME PRIVATE_API
*/
    static HMODULE s_hmod = LoadLibraryW(L"ComBase.dll");
    return s_hmod;
}

inline HRESULT __stdcall CoRegisterServerShutdownDelay(HANDLE stopEvent, DWORD milliseconds)
{
    static auto s_fn = reinterpret_cast<decltype(CoRegisterServerShutdownDelay)*>(GetProcAddress(GetComBaseModuleHandle(), MAKEINTRESOURCEA(69)));
    return s_fn ? s_fn(stopEvent, milliseconds) : E_UNEXPECTED;
}

int main()
{
    winrt::init_apartment();
    winrt::com_ptr<IGlobalOptions> globalOptions;
    if (SUCCEEDED(CoCreateInstance(CLSID_GlobalOptions, nullptr, CLSCTX_INPROC, IID_PPV_ARGS(&globalOptions))))
    {
        globalOptions->Set(COMGLB_EXCEPTION_HANDLING, COMGLB_EXCEPTION_DONOT_HANDLE_ANY);
        globalOptions->Set(COMGLB_RO_SETTINGS, COMGLB_FAST_RUNDOWN);
    }
    wil::unique_com_class_object_cookie widgetProviderFactory;
    // Create WidgetProvider factory
    auto factory = winrt::make<SingletonClassFactory<WidgetProvider>>();

    // CoRegister the WidgetProvider Factory with the GUID that was indicated in COM Server registration.
    winrt::check_hresult(CoRegisterClassObject(
        widget_provider_clsid,
        factory.get(),
        CLSCTX_LOCAL_SERVER,
        REGCLS_MULTIPLEUSE,
        widgetProviderFactory.put()));

    wil::unique_event shudownEvent(wil::EventOptions::None);
    winrt::check_hresult(CoRegisterServerShutdownDelay(shudownEvent.get(), 1 * 1000)); // 5 seconds

    DWORD index{};
    HANDLE events[] = { shudownEvent.get() };
    winrt::check_hresult(CoWaitForMultipleObjects(CWMO_DISPATCH_CALLS | CWMO_DISPATCH_WINDOW_MESSAGES, INFINITE, std::size(events), events, &index));

    return 0;
}
