#include "pch.h"
#include "Scenario1_ShortName.xaml.h"
#if __has_include("Scenario1_ShortName.g.cpp")
#include "Scenario1_ShortName.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;


namespace winrt::WinUI3TemplateCpp::implementation
{

    WinUI3TemplateCpp::MainPage Scenario1_ShortName::rootPage{ nullptr };

    Scenario1_ShortName::Scenario1_ShortName()
    {
        InitializeComponent();
        Scenario1_ShortName::rootPage = WinUI3TemplateCpp::MainPage::Current();
    }

    
    void Scenario1_ShortName::SuccessMessage_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        rootPage.NotifyUser(L"Everything was ok!", InfoBarSeverity::Success, true);
    }


    void Scenario1_ShortName::ErrorMessage_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        rootPage.NotifyUser(L"Everything was ok!", InfoBarSeverity::Error, true);
    }


    void Scenario1_ShortName::InformationalMessage_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        rootPage.NotifyUser(L"Everything was ok!", InfoBarSeverity::Informational, true);
    }


    void Scenario1_ShortName::ClearMessage_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        rootPage.NotifyUser(L"Everything was ok!", InfoBarSeverity::Informational, false);
    }
}
