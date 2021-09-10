//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#pragma once
#include "MainPage.g.h"

namespace winrt::WinUI3TemplateCpp::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage();
        static WinUI3TemplateCpp::MainPage Current() { return current; }
        static Windows::Foundation::Collections::IVector<WinUI3TemplateCpp::Scenario> Scenarios() { return scenariosInner; }
        void NavView_Loaded(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void NavView_ItemInvoked(Microsoft::UI::Xaml::Controls::NavigationView const& sender, Microsoft::UI::Xaml::Controls::NavigationViewItemInvokedEventArgs const& args);
        void NavView_Navigate(hstring navItemTag, Microsoft::UI::Xaml::Media::Animation::NavigationTransitionInfo const& transitionInfo);
        void NavView_BackRequested(Microsoft::UI::Xaml::Controls::NavigationView const& sender, Microsoft::UI::Xaml::Controls::NavigationViewBackRequestedEventArgs const& args);
        void ContentFrame_Navigated(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e);

        void NotifyUser(hstring const& strMessage, Microsoft::UI::Xaml::Controls::InfoBarSeverity const& severity);
        void UpdateStatus(hstring const& strMessage, Microsoft::UI::Xaml::Controls::InfoBarSeverity severity);

    private:
        static Windows::Foundation::Collections::IVector<Scenario> scenariosInner;
        static WinUI3TemplateCpp::MainPage current;
    };
}

namespace winrt::WinUI3TemplateCpp::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}
