#include "pch.h"

#include <wil/win32_helpers.h>
#include <iostream>
#include <winrt/Windows.ApplicationModel.Background.h>
#include <propkey.h> //PKEY properties
#include <propsys.h>
#include <ShObjIdl_core.h>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Microsoft::Windows::ApplicationModel::Resources;
using namespace Microsoft::Windows::PushNotifications;
using namespace Microsoft::Windows::AppNotifications;

// This function is intended to be called in the unpackaged scenario.
void SetDisplayNameAndIcon() noexcept try
{
    // Not mandatory, but it's highly recommended to specify AppUserModelId
    THROW_IF_FAILED(SetCurrentProcessExplicitAppUserModelID(L"TestPushAppId"));

    // Icon is mandatory
    winrt::com_ptr<IPropertyStore> propertyStore;
    wil::unique_hwnd hWindow{ GetConsoleWindow() };

    THROW_IF_FAILED(SHGetPropertyStoreForWindow(hWindow.get(), IID_PPV_ARGS(&propertyStore)));

    wil::unique_prop_variant propVariantIcon;
    wil::unique_cotaskmem_string sth = wil::make_unique_string<wil::unique_cotaskmem_string>(LR"(%SystemRoot%\system32\@WLOGO_96x96.png)");
    propVariantIcon.pwszVal = sth.release();
    propVariantIcon.vt = VT_LPWSTR;
    THROW_IF_FAILED(propertyStore->SetValue(PKEY_AppUserModel_RelaunchIconResource, propVariantIcon));

    // App name is not mandatory, but it's highly recommended to specify it
    wil::unique_prop_variant propVariantAppName;
    wil::unique_cotaskmem_string prodName = wil::make_unique_string<wil::unique_cotaskmem_string>(L"The Push Notification Demo App");
    propVariantAppName.pwszVal = prodName.release();
    propVariantAppName.vt = VT_LPWSTR;
    THROW_IF_FAILED(propertyStore->SetValue(PKEY_AppUserModel_RelaunchDisplayNameResource, propVariantAppName));
}
CATCH_LOG()

int main()
{
    init_apartment();
    ResourceManager manager(L"SelfContainedDeployment.pri");
    auto message = manager.MainResourceMap().GetValue(L"Resources/Description").ValueAsString();
    printf("%ls\n", message.c_str());

    PushNotificationManager pushManager{ PushNotificationManager::Default() };
    AppNotificationManager appManager{ AppNotificationManager::Default() };

    pushManager.Register();
    appManager.Register();
}
