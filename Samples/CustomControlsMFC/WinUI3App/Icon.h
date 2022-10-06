#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "Icon.g.h"

namespace winrt::WinUI3App::implementation
{
    struct Icon : IconT<Icon>
    {
        Icon();

        int32_t MyProperty();
        void MyProperty(int32_t value);

    };
}

namespace winrt::WinUI3App::factory_implementation
{
    struct Icon : IconT<Icon, implementation::Icon>
    {
    };
}
