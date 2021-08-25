#include "pch.h"
#include "MainPage.xaml.h"
#if __has_include("MainPage.g.cpp")
#include "MainPage.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Windows::UI::Xaml::Interop;
using namespace Microsoft::UI::Xaml::Media;
using namespace Microsoft::UI::Xaml::Media::Animation;
using namespace Microsoft::UI::Xaml::Controls;


namespace winrt::WinUI3TemplateCpp::implementation
{
    WinUI3TemplateCpp::MainPage MainPage::current{ nullptr };

    MainPage::MainPage()
    {
        InitializeComponent();
        MainPage::current = *this;
    }

    void MainPage::NavView_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        for (auto s : Scenarios())
        {
            FontIcon fontIcon{};
            fontIcon.FontFamily(winrt::Microsoft::UI::Xaml::Media::FontFamily(L"Segoe MDL2 Assets"));
            fontIcon.Glyph(L"\uE82D");

            NavigationViewItem navViewItem{};
            navViewItem.Content(winrt::box_value(s.Title));
            navViewItem.Tag(winrt::box_value(s.ClassName));
            navViewItem.Icon(fontIcon);
            NavView().MenuItems().Append(navViewItem);
        }

        // NavView doesn't load any page by default, so load home page.
        NavView().SelectedItem(NavView().MenuItems().GetAt(0));

        // If navigation occurs on SelectionChanged, this isn't needed.
        // Because we use ItemInvoked to navigate, we need to call Navigate
        // here to load the home page.
        if (Scenarios() != nullptr && Scenarios().Size() > 0)
        {
            //const EntranceNavigationTransitionInfo entranceNavTraInfo{};
            NavView_Navigate(Scenarios().GetAt(0).ClassName, nullptr);
        }
    }


    void MainPage::NavView_ItemInvoked(winrt::Microsoft::UI::Xaml::Controls::NavigationView const& sender, winrt::Microsoft::UI::Xaml::Controls::NavigationViewItemInvokedEventArgs const& args)
    {
        if (args.IsSettingsInvoked() == true)
        {
            hstring xamltypename = winrt::xaml_typename<SettingsPage>().Name;
            NavView_Navigate(xamltypename, args.RecommendedNavigationTransitionInfo());
        }
        else if (args.InvokedItemContainer() != nullptr)
        {
            hstring navItemTag = winrt::unbox_value<hstring>(args.InvokedItemContainer().Tag());
            if (navItemTag != L"")
            {
                NavView_Navigate(navItemTag, args.RecommendedNavigationTransitionInfo());
            }
        }
    }

    void MainPage::NavView_Navigate(hstring navItemTag, winrt::Microsoft::UI::Xaml::Media::Animation::NavigationTransitionInfo const& transitionInfo)
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

    void MainPage::NavView_BackRequested(winrt::Microsoft::UI::Xaml::Controls::NavigationView const& sender, winrt::Microsoft::UI::Xaml::Controls::NavigationViewBackRequestedEventArgs const& args)
    {
        if (ContentFrame().CanGoBack())
        {
            ContentFrame().GoBack();
        }
    }

    void MainPage::ContentFrame_Navigated(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e)
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
            for (auto item : NavView().MenuItems())
            {
                NavigationViewItem navViewItem = item.try_as<NavigationViewItem>();
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

    void MainPage::NotifyUser(hstring const& strMessage, InfoBarSeverity const& severity, bool isOpen)
    {
        
        UpdateStatus(strMessage, severity, isOpen);

        /* I got a ERROR winrt::impl::consume_Microsoft_UI_Dispatching_IDispatcherQueue2<D>::HasThreadAccess': 
        a function that returns 'auto' cannot be used before it is defined
        */
        /*
        if (this->DispatcherQueue().HasThreadAccess())
        {
            UpdateStatus(strMessage, severity, isOpen);
        }
        else
        {
            this->DispatcherQueue().TryEnqueue([strMessage, severity, isOpen, this]()
                {
                    UpdateStatus(strMessage, severity, isOpen);
                });
        }*/
    }

    void MainPage::UpdateStatus(hstring const& strMessage, Microsoft::UI::Xaml::Controls::InfoBarSeverity severity, bool isOpen)
    {
        infoBar().Message(strMessage);
        infoBar().IsOpen(isOpen);
        infoBar().Severity(severity);
    }
}


