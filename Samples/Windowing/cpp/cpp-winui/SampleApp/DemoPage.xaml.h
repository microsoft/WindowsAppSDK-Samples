// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include "DemoPage.g.h"


namespace winrt::SampleApp::implementation
{

    struct DemoPage : DemoPageT<DemoPage>
    {
	private:
		AppWindow m_mainAppWindow{ nullptr };

	public:
		DemoPage();
		void OnNavigatedTo(NavigationEventArgs const& e);

		void TitleBtn_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
		void SizeBtn_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void ClientSizeBtn_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
       
    };

}


namespace winrt::SampleApp::factory_implementation
{

    struct DemoPage : DemoPageT<DemoPage, implementation::DemoPage>
    {
    };

}
