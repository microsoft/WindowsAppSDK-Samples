﻿// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#include "pch.h"

#include "App.h"
#include "MainWindow.h"
#include "microsoft.ui.xaml.window.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Navigation;
using namespace winui_desktop_packaged_app_cpp;
using namespace winui_desktop_packaged_app_cpp::implementation;
using namespace winrt::Microsoft::Windows::ApplicationModel::Resources;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

/// <summary>
/// Initializes the singleton application object.  This is the first line of authored code
/// executed, and as such is the logical equivalent of main() or WinMain().
/// </summary>
App::App()
{
    InitializeComponent();

#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
    UnhandledException([this](IInspectable const&, UnhandledExceptionEventArgs const& e)
    {
        if (IsDebuggerPresent())
        {
            auto errorMessage = e.Message();
            __debugbreak();
        }
    });
#endif
}

App::~App()
{
#ifdef MRM_C_API_AVAILABLE // This API is not present in the current release package.
    MrmDestroyResourceManager(m_resourceManagerMrm);
#endif
}

/// <summary>
/// Invoked when the application is launched normally by the end user.  Other entry points
/// will be used such as when the application is launched to open a specific file.
/// </summary>
/// <param name="e">Details about the launch request and process.</param>
void App::OnLaunched(LaunchActivatedEventArgs const&)
{
    m_resourceManagerWinRT = ResourceManager(L"resources.pri");

#ifdef MRM_C_API_AVAILABLE // This API is not present in the current release package.
    check_hresult(MrmCreateResourceManager(L"resources.pri", &m_resourceManagerMrm));
#endif

    m_resourceManagerWinRT.ResourceNotFound([](ResourceManager const&, ResourceNotFoundEventArgs const& args)
        {
            // There could be a resource in a legacy resource file that we retrieve using the corresponding legacy resource loader. For example, the C#
            // app in this repo uses the .NET resource loader as its fallback. Instead, we just hardcode a resource value here.
            if (args.Name() == L"Resources/LegacyString")
            {
                auto candidate = ResourceCandidate(ResourceCandidateKind::String, L"Legacy Sample");
                args.SetResolvedCandidate(candidate);
            }
        });

    m_window = make<MainWindow>();

    auto window = m_window.as<winrt::winui_desktop_packaged_app_cpp::implementation::MainWindow>();

#ifdef MRM_C_API_AVAILABLE // This API is not present in the current release package.
    window->InitializeResourceLoaders(m_resourceManagerWinRT, m_resourceManagerMrm);
#else
    window->InitializeResourceLoaders(m_resourceManagerWinRT);
#endif

    m_window.Activate();
    m_window.Title(L"MRT Core C++ sample");

    HWND hwnd;
    m_window.as<IWindowNative>()->get_WindowHandle(&hwnd);
    // Window doesn't have Height and Weight properties in WInUI 3 Desktop yet,
    // to set the Width and Height you can use the Win32 API SetWindowPos.
    // However, you should have to take care of the DPI scale factor.
    SetWindowSize(hwnd, 550, 550);
}

void App::SetWindowSize(const HWND& hwnd, int width, int height)
{
    auto dpi = GetDpiForWindow(hwnd);
    float scalingFactor = static_cast<float>(dpi) / 96;
    RECT scale;
    scale.left = 0;
    scale.top = 0;
    scale.right = static_cast<LONG>(width * scalingFactor);
    scale.bottom = static_cast<LONG>(height * scalingFactor);
    SetWindowPos(hwnd, HWND_TOP, 0, 0, scale.right - scale.left, scale.bottom - scale.top, SWP_NOMOVE);
}
