﻿// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "WidgetImplBase.h"

struct WidgetProvider : winrt::implements<WidgetProvider, winrt::Microsoft::Windows::Widgets::Providers::IWidgetProvider,
    winrt::Microsoft::Windows::Widgets::Providers::IWidgetProvider2,
    winrt::Microsoft::Windows::Widgets::Providers::IWidgetProviderAnalytics,
    winrt::Microsoft::Windows::Widgets::Providers::IWidgetProviderErrors>
{
    WidgetProvider();

    /* IWidgetProvider required functions that need to be implemented */
    void CreateWidget(winrt::Microsoft::Windows::Widgets::Providers::WidgetContext WidgetContext);
    void DeleteWidget(winrt::hstring const& widgetId, winrt::hstring const& customState);
    void OnActionInvoked(winrt::Microsoft::Windows::Widgets::Providers::WidgetActionInvokedArgs actionInvokedArgs);
    void OnWidgetContextChanged(winrt::Microsoft::Windows::Widgets::Providers::WidgetContextChangedArgs contextChangedArgs);
    void Activate(winrt::Microsoft::Windows::Widgets::Providers::WidgetContext widgetContext);
    void Deactivate(winrt::hstring widgetId);
    /* IWidgetProvider required functions that need to be implemented */

    /* IWidgetProvider2 required functions that need to be implemented */
    void OnCustomizationRequested(winrt::WidgetCustomizationRequestedArgs const& args);
    /* IWidgetProvider2 required functions that need to be implemented */

    /* IWidgetProviderAnalytics required functions that need to be implemented */
    void OnAnalyticsInfoReported(winrt::WidgetAnalyticsInfoReportedArgs const& eventNotifiedArgs);
    /* IWidgetProviderAnalytics required functions that need to be implemented */

    /* IWidgetProviderErrors required functions that need to be implemented */
    void OnErrorInfoReported(winrt::WidgetErrorInfoReportedArgs const& eventNotifiedArgs);
    /* IWidgetProviderErrors required functions that need to be implemented */

private:
    void RecoverRunningWidgets();

    std::shared_ptr<WidgetImplBase> InitializeWidgetInternal(winrt::Microsoft::Windows::Widgets::Providers::WidgetContext widgetContext, winrt::hstring state);
    std::shared_ptr<WidgetImplBase> FindRunningWidget(winrt::hstring widgetId);

    static std::unordered_map<winrt::hstring, std::shared_ptr<WidgetImplBase>> m_runningWidgetImpl;
};
