// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

class WidgetImplBase
{
public:
    WidgetImplBase() {};

    winrt::hstring Id() { return m_id; };
    winrt::hstring State() { return m_state; };
    bool IsActivated() { return m_isActivated; };

    void Id(winrt::hstring const& id) { m_id = id; };
    void State(winrt::hstring const& state) { m_state = state; };
    void IsActivated(bool isActivated) { m_isActivated = isActivated; };

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
