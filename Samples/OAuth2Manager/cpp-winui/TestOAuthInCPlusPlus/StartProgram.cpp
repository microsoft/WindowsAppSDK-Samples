// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.
#include "pch.h"
#include "App.xaml.h"
#include "MainWindow.xaml.h"
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Microsoft.Windows.AppLifecycle.h>
#include <winrt/Windows.ApplicationModel.Activation.h>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Microsoft::UI::Dispatching;
using namespace Microsoft::Windows::AppLifecycle;
using namespace Windows::ApplicationModel::Activation;
using namespace winrt::TestOAuthInCPlusPlus::implementation;

static Microsoft::UI::Dispatching::DispatcherQueue dispatcherQueue{ nullptr };

void OnActivated(Windows::Foundation::IInspectable const&, AppActivationArguments const& args)
{
    if (args.Kind() == ExtendedActivationKind::Protocol)
    {
        auto protocolArgs = args.Data().as<ProtocolActivatedEventArgs>();
        dispatcherQueue.TryEnqueue([protocolArgs]()
            {
                auto absUri = protocolArgs.Uri().AbsoluteUri();
                std::wstring absUriStr = absUri.c_str(); // Convert winrt::hstring to std::wstring
                auto mainWindow = App::Window().try_as<TestOAuthInCPlusPlus::MainWindow>();
                if (absUriStr.find(L"//oauthcallback") != std::wstring::npos)
                    mainWindow.OnUriCallback(protocolArgs.Uri());
            });
    }
}

static IAsyncOperation<bool> DecideRedirection()
{
    auto mainInstance = AppInstance::FindOrRegisterForKey(L"main");
    auto activatedEventArgs = AppInstance::GetCurrent().GetActivatedEventArgs();

    if (!mainInstance.IsCurrent())
    {
        co_await mainInstance.RedirectActivationToAsync(activatedEventArgs);
        co_return true;
    }

    co_return false;
}

int __stdcall wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
{
    winrt::init_apartment(winrt::apartment_type::multi_threaded);

    AppInstance::GetCurrent().Activated(OnActivated);

    bool isRedirect = DecideRedirection().get();

    if (!isRedirect)
    {
        Microsoft::UI::Xaml::Application::Start([](auto&&)
            {
                dispatcherQueue = Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread();
                ::winrt::make<winrt::TestOAuthInCPlusPlus::implementation::App>();
            });
    }

    return 0;
}
