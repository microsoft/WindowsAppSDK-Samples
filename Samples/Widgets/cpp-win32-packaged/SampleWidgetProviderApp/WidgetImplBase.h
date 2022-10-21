// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

class WidgetImplBase
{
public:
    WidgetImplBase() = default;
    virtual ~WidgetImplBase() = default;

    winrt::hstring Id() const noexcept { return m_id; };
    winrt::hstring State() const noexcept { return m_state; };
    bool IsActivated() const noexcept { return m_isActivated; };

    void State(winrt::hstring const& state) { m_state = state; };

    virtual void Activate(winrt::Microsoft::Windows::Widgets::Providers::WidgetContext widgetContext) {};
    virtual void Deactivate(winrt::hstring) {};
    virtual void OnActionInvoked(winrt::WidgetActionInvokedArgs actionInvokedArgs) {};
    virtual void OnWidgetContextChanged(winrt::WidgetContextChangedArgs contextChangedArgs) {};
    virtual winrt::hstring GetTemplateForWidget() = 0;
    virtual winrt::hstring GetDataForWidget() = 0;

protected:
    WidgetImplBase(winrt::hstring const& id, winrt::hstring const& state) : m_id{ id }, m_state{ state } {};
    winrt::hstring m_state{};
    winrt::hstring m_id{};
    bool m_isActivated{ false };
};
