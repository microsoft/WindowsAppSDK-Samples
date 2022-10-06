#include "pch.h"
#include "Icon.h"
#if __has_include("Icon.g.cpp")
#include "Icon.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::WinUI3App::implementation
{
    Icon::Icon()
    {
        InitializeComponent();
    }

    int32_t Icon::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void Icon::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }
}
