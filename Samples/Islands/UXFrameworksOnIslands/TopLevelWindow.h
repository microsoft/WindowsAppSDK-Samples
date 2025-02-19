// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

#include "IFrame.h"
#include "IFrameHost.h"
#include "IFocusHost.h"
#include "AutomationFragmentRoot.h"
#include "EventRevokers.h"

// Wrapper class for our top level window, message loop, and message processing
class TopLevelWindow :
    public AutomationHelpers::IAutomationFragmentCallbackHandler,
    public AutomationHelpers::IAutomationFragmentRootCallbackHandler,
    public AutomationHelpers::IAutomationExternalChildCallbackHandler,
    public IFrameHost,
    public IFocusHost
{
public:
    TopLevelWindow(
        const winrt::DispatcherQueue& queue);

    void ConnectFrameToWindow(
        IFrame* frame);

    // AutomationHelpers::IAutomationFragmentCallbackHandler implementation
    winrt::Windows::Graphics::RectInt32 GetBoundingRectangleInScreenCoordinatesForAutomation(
        ::IUnknown const* const sender
        ) const override;

    void HandleSetFocusForAutomation(
        ::IUnknown const* const sender
        ) override;

    // AutomationHelpers::IAutomationFragmentRootCallbackHandler implementation
    winrt::com_ptr<::IRawElementProviderFragment> GetFragmentFromPointInScreenCoordinatesForAutomation(
        double x,
        double y,
        ::IUnknown const* const sender
        ) const override;

    winrt::com_ptr<::IRawElementProviderFragment> GetFragmentInFocusForAutomation(
        ::IUnknown const* const sender
        ) const override;

    // AutomationHelpers::IAutomationExternalChildCallbackHandler implementation
    winrt::com_ptr<::IRawElementProviderFragment> GetFirstChildFragmentForAutomation(
        ::IUnknown const* const sender
        ) const override;

    winrt::com_ptr<::IRawElementProviderFragment> GetLastChildFragmentForAutomation(
        ::IUnknown const* const sender
        ) const override;

    // IFrameHost implementation
    winrt::com_ptr<::IRawElementProviderFragmentRoot> GetFragmentRootProviderForChildFrame(
        _In_ IFrame const* const sender
        ) const override;

    winrt::com_ptr<::IRawElementProviderFragment> GetNextSiblingProviderForChildFrame(
        _In_ IFrame const* const sender
        ) const override;

    winrt::com_ptr<::IRawElementProviderFragment> GetParentProviderForChildFrame(
        _In_ IFrame const* const sender
        ) const override;

    winrt::com_ptr<::IRawElementProviderFragment> GetPreviousSiblingProviderForChildFrame(
        _In_ IFrame const* const sender
        ) const override;

    // IFocusHost implementation
    void EnsureWin32Focus() const override;

private:
    static LRESULT CALLBACK WindowProc(
        HWND hwnd, 
        UINT message, 
        WPARAM wparam, 
        LPARAM lparam);

    LRESULT MessageHandler(
        UINT message, 
        WPARAM wparam, 
        LPARAM lparam);

    void RegisterWindowClass();

    void InitializeWindow();

    void InitializeBridge(
        const winrt::DispatcherQueue& queue);

    void InitializeAutomation();

private:
    static constexpr wchar_t k_windowClassName[] = L"UXFrameworksOnIslandsClass";
    static constexpr wchar_t k_windowName[] = L"UXFrameworksOnIslands";
    static constexpr wchar_t k_windowAutomationName[] = L"UXFrameworksOnIslands TopLevelWindow";

    HWND m_hwnd = NULL;

    winrt::AppWindow m_window = nullptr;
    AppWindowClosing_revoker m_closingRevoker{};

    winrt::ReadOnlyDesktopSiteBridge m_bridge = nullptr;

    bool m_windowAutomationRootRegisteredWithUIA = false;
    std::unique_ptr<AutomationHelpers::AutomationCallbackRevoker> m_fragmentRevoker = nullptr;
    std::unique_ptr<AutomationHelpers::AutomationCallbackRevoker> m_fragmentRootRevoker = nullptr;
    std::unique_ptr<AutomationHelpers::AutomationCallbackRevoker> m_externalChildRevoker = nullptr;
    winrt::com_ptr<AutomationHelpers::AutomationFragmentRoot> m_windowAutomationRoot = nullptr;

    IFrame* m_frame = nullptr;
};
