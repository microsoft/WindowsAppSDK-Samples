#pragma once

#include "MainPage.g.h"

namespace winrt::WinUI3TemplateCpp::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage();
        static WinUI3TemplateCpp::MainPage Current() { return current; }
        static Windows::Foundation::Collections::IVector<WinUI3TemplateCpp::Scenario> Scenarios() { return scenariosInner; }
        void NavView_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void NavView_ItemInvoked(winrt::Microsoft::UI::Xaml::Controls::NavigationView const& sender, winrt::Microsoft::UI::Xaml::Controls::NavigationViewItemInvokedEventArgs const& args);
        void NavView_Navigate(hstring navItemTag, winrt::Microsoft::UI::Xaml::Media::Animation::NavigationTransitionInfo const& transitionInfo);
        void NavView_BackRequested(winrt::Microsoft::UI::Xaml::Controls::NavigationView const& sender, winrt::Microsoft::UI::Xaml::Controls::NavigationViewBackRequestedEventArgs const& args);
        void ContentFrame_Navigated(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e);

        void NotifyUser(hstring const& strMessage, Microsoft::UI::Xaml::Controls::InfoBarSeverity const& severity, bool isOpen);
        void UpdateStatus(hstring const& strMessage, Microsoft::UI::Xaml::Controls::InfoBarSeverity severity, bool isOpen);

    private:
        static winrt::Windows::Foundation::Collections::IVector<Scenario> scenariosInner;
        static WinUI3TemplateCpp::MainPage current;
    };
}

namespace winrt::WinUI3TemplateCpp::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}
