// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

struct CountingWidgetInfo
{
	winrt::hstring m_widgetId = L"";
	int m_clickCount = 0;
	bool m_isActivated = false;
};

struct WidgetProvider : winrt::implements<WidgetProvider, winrt::Microsoft::Windows::Widgets::Providers::IWidgetProvider>
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

private:
	void RecoverRunningWidgets();
	winrt::hstring GetTemplateForWidget();
	winrt::hstring GetDataForWidget(winrt::hstring const& widgetId);
	winrt::hstring GetStateForWidget(winrt::hstring const& widgetId);

	std::shared_ptr<CountingWidgetInfo> FindRunningWidget(winrt::hstring widgetId);

	static std::unordered_map<winrt::hstring, std::shared_ptr<CountingWidgetInfo>> m_runningWidgets;
};