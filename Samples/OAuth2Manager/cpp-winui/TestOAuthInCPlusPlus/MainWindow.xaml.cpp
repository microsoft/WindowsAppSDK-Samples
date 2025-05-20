// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.
#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif
#include "winrt/Windows.Foundation.h"
#include "winrt/Microsoft.UI.Xaml.Controls.h"
#include <winrt/Microsoft.UI.Dispatching.h>
#include "winrt/Microsoft.Security.Authentication.OAuth.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace winrt::Microsoft::UI::Dispatching;
using namespace Microsoft::Security::Authentication::OAuth;
// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::TestOAuthInCPlusPlus::implementation
{
    MainWindow::MainWindow()
    {
        auto parentWindow = ::GetForegroundWindow();
        parentWindowId = winrt::Microsoft::UI::WindowId{ reinterpret_cast<uint64_t>(parentWindow) };
        dispatcherQueue = Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread();
    }

    // Asynchronous function that handles button click events
    winrt::Windows::Foundation::IAsyncAction MainWindow::authCodeGrantWithRedirectButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
         
		auto clientID = L"your client id";
        auto clientSecret = L"your client secret";

        AuthRequestParams authRequestParams = AuthRequestParams::CreateForAuthorizationCodeRequest(clientID, Uri(L"ms-testoauthincpp-launch://oauthcallback"));
        authRequestParams.Scope(L"read:user user:email");
        AuthRequestResult authRequestResult = co_await OAuth2Manager::RequestAuthWithParamsAsync(parentWindowId, Uri(L"https://github.com/login/oauth/authorize"), authRequestParams);
        AuthResponse authResponse = authRequestResult.Response();

        if (authResponse)
        {
            TokenRequestParams tokenRequestParams = TokenRequestParams::CreateForAuthorizationCodeRequest(authResponse);
            ClientAuthentication clientAuth = ClientAuthentication::CreateForBasicAuthorization(clientID, clientSecret);
            TokenRequestResult tokenRequestResult = co_await OAuth2Manager::RequestTokenAsync(Uri(L"https://github.com/login/oauth/access_token"), tokenRequestParams, clientAuth);

            auto accessToken = tokenRequestResult.Response().AccessToken();

            // Ensure this runs on the UI thread
            dispatcherQueue.TryEnqueue([this, accessToken, authRequestResult]()
                {
                    outputTextBlock().Text(L"Auth code type TokenResponse : Token: " + accessToken + L"\nAuth res: " + authRequestResult.ResponseUri().RawUri());
                });
        }
    }

    void MainWindow::OnUriCallback(Uri const& responseUri)
    {
        if (!OAuth2Manager::CompleteAuthRequest(responseUri))
        {
            // The response is either invalid or does not correspond to any pending auth requests.
        }
    }
}
