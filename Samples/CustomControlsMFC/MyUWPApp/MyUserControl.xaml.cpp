// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#include "pch.h"
#include "MyUserControl.xaml.h"
#if __has_include("MyUserControl.g.cpp")
#include "MyUserControl.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::MyUWPApp::implementation
{
    MyUserControl::MyUserControl()
    {
        InitializeComponent();
    }

    int32_t MyUserControl::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void MyUserControl::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void MyUserControl::myButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        myButton().Content(box_value(L"Clicked"));
    }
}
