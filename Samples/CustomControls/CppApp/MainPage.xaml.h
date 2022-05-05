// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "MainPage.g.h"

namespace winrt::CppApp::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage();
        static CppApp::MainPage Current() { return current; }
        static Windows::Foundation::Collections::IVector<CppApp::Scenario> Scenarios() { return scenariosInner; }
        void NotifyUser(hstring const& strMessage, Microsoft::UI::Xaml::Controls::InfoBarSeverity const& severity);
        void UpdateStatus(hstring const& strMessage, Microsoft::UI::Xaml::Controls::InfoBarSeverity severity);
        void NavView_Loaded(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void NavView_ItemInvoked(Microsoft::UI::Xaml::Controls::NavigationView const& sender, Microsoft::UI::Xaml::Controls::NavigationViewItemInvokedEventArgs const& args);
        void NavView_Navigate(hstring navItemTag, Microsoft::UI::Xaml::Media::Animation::NavigationTransitionInfo const& transitionInfo);
        void NavView_BackRequested(Microsoft::UI::Xaml::Controls::NavigationView const& sender, Microsoft::UI::Xaml::Controls::NavigationViewBackRequestedEventArgs const& args);
        void ContentFrame_Navigated(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e);

    private:
        static Windows::Foundation::Collections::IVector<Scenario> scenariosInner;
        static CppApp::MainPage current;
    };
}

namespace winrt::CppApp::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}
