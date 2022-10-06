// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#include "pch.h"

#include "App.xaml.h"
#include "MainWindow.xaml.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Navigation;
using namespace MyUWPApp;
using namespace MyUWPApp::implementation;

//#include "pch.h"
//#include "App.h"
#include "App.g.cpp"
using namespace winrt;
using namespace Windows::UI::Xaml;
namespace winrt::MyUWPApp::implementation
{
    App::App()
    {
        Initialize();
        AddRef();
        m_inner.as<::IUnknown>()->Release();
    }
    App::~App()
    {
        Close();
    }
}
