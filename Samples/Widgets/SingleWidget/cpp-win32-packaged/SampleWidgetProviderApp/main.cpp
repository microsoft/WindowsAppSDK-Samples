// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "WidgetProvider.h"
#include <mutex>

// This GUID is the same GUID thas was provided in the 
// registration of the COM Server and Class Id in the .appxmanifest.
static constexpr GUID widget_provider_clsid
{ /* 8AFD9DB6-84A7-4425-BD7E-761D9C063B40 */
    0x8afd9db6, 0x84a7, 0x4425, { 0xbd, 0x7e, 0x76, 0x1d, 0x9c, 0x6, 0x3b, 0x40 }
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

        if (!instance)
        {
            instance = winrt::make_self<T>();
        }

        return instance.as(iid, result);
    }

    STDMETHODIMP LockServer(BOOL) noexcept final
    {
        return S_OK;
    }

private:
    winrt::com_ptr<T> instance{ nullptr };
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

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
