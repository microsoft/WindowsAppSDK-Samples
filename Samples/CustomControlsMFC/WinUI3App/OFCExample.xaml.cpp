﻿#include "pch.h"
#include "OFCExample.xaml.h"
#if __has_include("OFCExample.g.cpp")
#include "OFCExample.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::WinUI3App::implementation
{
    OFCExample::OFCExample()
    {
        InitializeComponent();
    }

    int32_t OFCExample::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void OFCExample::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }
}
