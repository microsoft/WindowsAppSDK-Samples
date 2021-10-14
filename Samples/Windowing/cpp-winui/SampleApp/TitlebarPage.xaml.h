// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include "TitlebarPage.g.h"

namespace winrt::SampleApp::implementation
{

	struct TitlebarPage : TitlebarPageT<TitlebarPage>
	{
    public:
		TitlebarPage();
		void OnNavigatedTo(NavigationEventArgs const& e);

		bool m_brandTitleBar = false;
		bool m_customTitleBar = false;

		void TitlebarBrandingBtn_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
		void TitlebarCustomBtn_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
		void ResetTitlebarBtn_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
		void SetCustomTitleBarDragRegion();
		void AppWindowChangedHandler(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Windowing::AppWindowChangedEventArgs const& e);

	private:
		AppWindow m_appWindow{ nullptr };
		SampleApp::MainWindow m_mainWindow{ nullptr };
		winrt::event_token m_changedToken;
	};
}


namespace winrt::SampleApp::factory_implementation
{

	struct TitlebarPage : TitlebarPageT<TitlebarPage, implementation::TitlebarPage>
	{
	};

}
