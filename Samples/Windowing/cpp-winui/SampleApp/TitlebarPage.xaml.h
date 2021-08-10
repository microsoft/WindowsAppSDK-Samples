#pragma once

#include "TitlebarPage.g.h"

namespace winrt::SampleApp::implementation
{

	struct TitlebarPage : TitlebarPageT<TitlebarPage>
	{
		

	private:
		AppWindow m_appWindow{ nullptr };
		SampleApp::MainWindow m_mainWindow{ nullptr };


	public:
		TitlebarPage();
		void OnNavigatedTo(NavigationEventArgs const& e);

		bool m_brandTitleBar = false;
		bool m_customTitleBar = false;

		void TitlebarBrandingBtn_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
		void TitlebarCustomBtn_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
		void ResetTitlebarBtn_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
	};

}


namespace winrt::SampleApp::factory_implementation
{

	struct TitlebarPage : TitlebarPageT<TitlebarPage, implementation::TitlebarPage>
	{
	};

}
