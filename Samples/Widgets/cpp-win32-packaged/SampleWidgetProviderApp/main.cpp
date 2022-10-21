// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "WidgetProvider.h"
#include <mutex>

// This GUID is the same GUID that was provided in the 
// registration of the COM Server and Class Id in the .appxmanifest.
static constexpr GUID widget_provider_clsid
{ /* 101D03A3-6FC8-4887-9B64-310A7B164319 */
    0x101d03a3, 0x6fc8, 0x4887, {0x9b, 0x64, 0x31, 0x0a, 0x7b, 0x16, 0x43, 0x19}
};

wil::unique_event g_shudownEvent(wil::EventOptions::None);

void SignalLocalServerShutdown()
{
    g_shudownEvent.SetEvent();
}

// This is implementation of a ClassFactory that will instantiate WidgetProvider
// that will interact with the Widget Service.
template <typename T>
struct SingletonClassFactory : winrt::implements<SingletonClassFactory<T>, IClassFactory, winrt::no_module_lock>
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
        
        return winrt::make<T>().as(iid, result);
    }

    STDMETHODIMP LockServer(BOOL) noexcept final
    {
        return S_OK;
    }

private:
    std::mutex mutex;
};

int main()
{
    winrt::init_apartment();

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

    DWORD index{};
    HANDLE events[] = { g_shudownEvent.get() };
    winrt::check_hresult(CoWaitForMultipleObjects(CWMO_DISPATCH_CALLS | CWMO_DISPATCH_WINDOW_MESSAGES, INFINITE,
        static_cast<ULONG>(std::size(events)), events, &index));

    return 0;
}
