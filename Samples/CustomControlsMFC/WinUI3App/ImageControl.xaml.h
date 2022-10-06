#pragma once

#include "winrt/Microsoft.UI.Xaml.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
#include "winrt/Microsoft.UI.Xaml.Controls.Primitives.h"
#include "ImageControl.g.h"

namespace winrt::WinUI3App::implementation
{
    struct ImageControl : ImageControlT<ImageControl>
    {
        ImageControl();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void imageLink(const winrt::hstring& str);
        winrt::hstring imageLink();
    };
}

namespace winrt::WinUI3App::factory_implementation
{
    struct ImageControl : ImageControlT<ImageControl, implementation::ImageControl>
    {
    };
}
