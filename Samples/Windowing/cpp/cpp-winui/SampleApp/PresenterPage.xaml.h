// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include "PresenterPage.g.h"


namespace winrt::SampleApp::implementation
{

	struct PresenterPage : PresenterPageT<PresenterPage>
	{
	private:
		AppWindow m_mainAppWindow{ nullptr };
		hstring title = L"WinUI Desktop Sample App";

	public:
		PresenterPage();
		void OnNavigatedTo(NavigationEventArgs const& e);
		void SwitchPresenter(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
	};
}


namespace winrt::SampleApp::factory_implementation
{

	struct PresenterPage : PresenterPageT<PresenterPage, implementation::PresenterPage>
	{
	};

}
