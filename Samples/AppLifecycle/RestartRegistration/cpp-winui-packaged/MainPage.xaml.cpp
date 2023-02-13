// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "MainPage.xaml.h"
#if __has_include("MainPage.g.cpp")
#include "MainPage.g.cpp"
#endif

namespace winrt
{
    using namespace Microsoft::UI::Xaml;
    using namespace Microsoft::UI::Xaml::Controls;
    using namespace Microsoft::UI::Xaml::Media;
    using namespace Microsoft::UI::Xaml::Media::Animation;
    using namespace Microsoft::UI::Xaml::Navigation;
    using namespace Windows::UI::Xaml::Interop;
}

namespace winrt::cpp_winui_packaged::implementation
{
    cpp_winui_packaged::MainPage MainPage::current{ nullptr };

    MainPage::MainPage()
    {
        InitializeComponent();
        MainPage::current = *this;
    }

    void MainPage::NotifyUser(hstring const& strMessage, InfoBarSeverity const& severity)
    {
        // If called from the UI thread, then update immediately.
        // Otherwise, schedule a task on the UI thread to perform the update.
        if (this->DispatcherQueue().HasThreadAccess())
        {
            UpdateStatus(strMessage, severity);
        }
        else
        {
            DispatcherQueue().TryEnqueue([strongThis = get_strong(), this, strMessage, severity]
                { UpdateStatus(strMessage, severity); });
        }
    }

    void MainPage::UpdateStatus(hstring const& strMessage, InfoBarSeverity severity)
    {
        infoBar().Message(strMessage);
        infoBar().IsOpen(!strMessage.empty());
        infoBar().Severity(severity);
    }

    void MainPage::NavView_Loaded(IInspectable const&, RoutedEventArgs const&)
    {
        for (auto&& s : Scenarios())
        {
            FontIcon fontIcon{};
            fontIcon.FontFamily(winrt::FontFamily(L"Segoe MDL2 Assets"));
            fontIcon.Glyph(L"\uE82D");

            NavigationViewItem navViewItem{};
            navViewItem.Content(box_value(s.Title));
            navViewItem.Tag(box_value(s.ClassName));
            navViewItem.Icon(fontIcon);
            NavView().MenuItems().Append(navViewItem);
        }

        // NavView doesn't load any page by default, so load home page.
        NavView().SelectedItem(NavView().MenuItems().GetAt(0));

        // If navigation occurs on SelectionChanged, this isn't needed.
        // Because we use ItemInvoked to navigate, we need to call Navigate
        // here to load the home page.
        if (Scenarios().Size() > 0)
        {
            NavView_Navigate(Scenarios().GetAt(0).ClassName, nullptr);
        }
    }


    void MainPage::NavView_ItemInvoked(NavigationView const&, NavigationViewItemInvokedEventArgs const& args)
    {
        if (args.IsSettingsInvoked() == true)
        {
            hstring xamltypename = winrt::xaml_typename<SettingsPage>().Name;
            NavView_Navigate(xamltypename, args.RecommendedNavigationTransitionInfo());
        }
        else if (args.InvokedItemContainer() != nullptr)
        {
            auto navItemTag = winrt::unbox_value<hstring>(args.InvokedItemContainer().Tag());
            if (navItemTag != L"")
            {
                NavView_Navigate(navItemTag, args.RecommendedNavigationTransitionInfo());
            }
        }
    }

    void MainPage::NavView_Navigate(hstring navItemTag, NavigationTransitionInfo const&)
    {
        TypeName pageType;

        if (navItemTag == winrt::xaml_typename<SettingsPage>().Name)
        {
            pageType = winrt::xaml_typename<SettingsPage>();
        }
        else
        {
            pageType.Name = navItemTag;
            pageType.Kind = TypeKind::Metadata;
        }

        // Get the page type before navigation so you can prevent duplicate
        // entries in the backstack.
        TypeName prePageType = ContentFrame().CurrentSourcePageType();

        // Only navigate if the selected page isn't currently loaded.
        if (prePageType.Name != pageType.Name)
        {
            DrillInNavigationTransitionInfo drillIn;
            ContentFrame().Navigate(pageType, nullptr, drillIn);
        }
    }

    void MainPage::NavView_BackRequested(NavigationView const&, NavigationViewBackRequestedEventArgs const&)
    {
        if (ContentFrame().CanGoBack())
        {
            ContentFrame().GoBack();
        }
    }

    void MainPage::ContentFrame_Navigated(IInspectable const&, NavigationEventArgs const& e)
    {
        NavView().IsBackEnabled(ContentFrame().CanGoBack());

        if (ContentFrame().SourcePageType().Name == winrt::xaml_typename<SettingsPage>().Name)
        {
            // SettingsItem is not part of NavView.MenuItems, and doesn't have a Tag.
            NavView().SelectedItem((NavView().SettingsItem().as<NavigationViewItem>()));
            NavView().Header(winrt::box_value(L"Settings"));
        }
        else
        {
            for (auto&& item : NavView().MenuItems())
            {
                auto navViewItem = item.try_as<NavigationViewItem>();
                if (navViewItem != nullptr && 
                    winrt::unbox_value<hstring>(navViewItem.Tag()) == e.SourcePageType().Name)
                {
                    NavView().SelectedItem(navViewItem);
                    NavView().Header(navViewItem.Content());
                    break;
                }
            }
        }
    }
}
