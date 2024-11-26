// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

#define EVENT_REVOKER_IMPLEMENTATION(WinRTNamespace, WinRTObject, EventMethod) \
struct WinRTObject##EventMethod##_revoker \
{ \
    explicit WinRTObject##EventMethod##_revoker() noexcept = default; \
\
    WinRTObject##EventMethod##_revoker(WinRTObject##EventMethod##_revoker const&) = delete; \
    WinRTObject##EventMethod##_revoker& operator=(WinRTObject##EventMethod##_revoker const&) = delete; \
\
    WinRTObject##EventMethod##_revoker(WinRTObject##EventMethod##_revoker&&) = delete; \
    WinRTObject##EventMethod##_revoker& operator=(WinRTObject##EventMethod##_revoker&& other) noexcept \
    { \
        if (&other != this) \
        { \
            Revoke(); \
\
            m_object = other.m_object; \
            m_token = other.m_token; \
\
            other.m_object = nullptr; \
            other.m_token.value = 0; \
        } \
\
        return *this; \
    } \
\
    WinRTObject##EventMethod##_revoker(WinRTNamespace::WinRTObject const& object, winrt::event_token const& token) noexcept : m_object{ object }, m_token{ token } {} \
    ~WinRTObject##EventMethod##_revoker() noexcept { Revoke(); } \
\
    void Revoke() noexcept \
    { \
        if (nullptr != m_object) \
        { \
            m_object.EventMethod(m_token); \
            m_object = nullptr; \
            m_token.value = 0; \
        } \
    } \
\
private: \
    WinRTNamespace::WinRTObject m_object{ nullptr }; \
    winrt::event_token m_token{}; \
};

EVENT_REVOKER_IMPLEMENTATION(winrt::Microsoft::UI::Content, ContentIsland, AutomationProviderRequested)
EVENT_REVOKER_IMPLEMENTATION(winrt::Microsoft::UI::Content, ContentIsland, StateChanged)
EVENT_REVOKER_IMPLEMENTATION(winrt::Microsoft::UI::Content, ContentIslandEnvironment, StateChanged)
EVENT_REVOKER_IMPLEMENTATION(winrt::Microsoft::UI::Content, ChildContentLink, ParentAutomationProviderRequested)
EVENT_REVOKER_IMPLEMENTATION(winrt::Microsoft::UI::Content, ChildContentLink, NextSiblingAutomationProviderRequested)
EVENT_REVOKER_IMPLEMENTATION(winrt::Microsoft::UI::Content, ChildContentLink, PreviousSiblingAutomationProviderRequested)
EVENT_REVOKER_IMPLEMENTATION(winrt::Microsoft::UI::Content, ChildContentLink, FragmentRootAutomationProviderRequested)
EVENT_REVOKER_IMPLEMENTATION(winrt::Microsoft::UI::Windowing, AppWindow, Closing)
