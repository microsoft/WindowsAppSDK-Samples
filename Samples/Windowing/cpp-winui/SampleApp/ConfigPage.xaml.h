// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include "ConfigPage.g.h"


namespace winrt::SampleApp::implementation
{

struct ConfigPage : ConfigPageT<ConfigPage>
{
public:
	ConfigPage();
	void OnNavigatedTo(NavigationEventArgs const& e);

	void ChangeConfiguration(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
	void ChangeWindowStyle(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

private:
	AppWindow m_mainAppWindow{ nullptr };
};

static void VisibilityChangedHandler(winrt::Windows::Foundation::IInspectable const& sender, winrt::WindowVisibilityChangedEventArgs const& e);

}


namespace winrt::SampleApp::factory_implementation
{

struct ConfigPage : ConfigPageT<ConfigPage, implementation::ConfigPage>
{
};

}
