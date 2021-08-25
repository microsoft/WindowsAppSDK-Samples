#pragma once

#include "Scenario1_ShortName.g.h"

namespace winrt::WinUI3TemplateCpp::implementation
{
    struct Scenario1_ShortName : Scenario1_ShortNameT<Scenario1_ShortName>
    {
        Scenario1_ShortName();
        void SuccessMessage_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void ErrorMessage_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void InformationalMessage_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void ClearMessage_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
    private:
        static WinUI3TemplateCpp::MainPage rootPage;
    };
}

namespace winrt::WinUI3TemplateCpp::factory_implementation
{
    struct Scenario1_ShortName : Scenario1_ShortNameT<Scenario1_ShortName, implementation::Scenario1_ShortName>
    {
    };
}
