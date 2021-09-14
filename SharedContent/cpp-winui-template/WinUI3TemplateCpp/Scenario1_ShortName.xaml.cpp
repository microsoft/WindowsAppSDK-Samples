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

#include "pch.h"
#include "Scenario1_ShortName.xaml.h"
#if __has_include("Scenario1_ShortName.g.cpp")
#include "Scenario1_ShortName.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Windows::Foundation;


namespace winrt::WinUI3TemplateCpp::implementation
{

    MainPage Scenario1_ShortName::rootPage{ nullptr };

    Scenario1_ShortName::Scenario1_ShortName()
    {
        InitializeComponent();
        Scenario1_ShortName::rootPage = MainPage::Current();
    }

    void Scenario1_ShortName::NotifyUser(hstring const& strMessage, InfoBarSeverity const& severity)
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

    void Scenario1_ShortName::UpdateStatus(hstring const& strMessage, InfoBarSeverity severity)
    {
        infoBar().Message(strMessage);
        infoBar().IsOpen(!strMessage.empty());
        infoBar().Severity(severity);
    }
    
    void Scenario1_ShortName::SuccessMessage_Click(IInspectable const&, RoutedEventArgs const&)
    {
        NotifyUser(L"Everything was ok!", InfoBarSeverity::Success);
    }


    void Scenario1_ShortName::ErrorMessage_Click(IInspectable const&, RoutedEventArgs const&)
    {
        NotifyUser(L"Something went wrong.", InfoBarSeverity::Error);
    }


    void Scenario1_ShortName::InformationalMessage_Click(IInspectable const&, RoutedEventArgs const&)
    {
        NotifyUser(L"This is the informational bar.", InfoBarSeverity::Informational);
    }


    void Scenario1_ShortName::ClearMessage_Click(IInspectable const&, RoutedEventArgs const&)
    {
        NotifyUser(L"", InfoBarSeverity::Informational);
    }
}
