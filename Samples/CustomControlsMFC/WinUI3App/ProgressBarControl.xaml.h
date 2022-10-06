#pragma once

#include "winrt/Microsoft.UI.Xaml.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
#include "winrt/Microsoft.UI.Xaml.Controls.Primitives.h"
#include "ProgressBarControl.g.h"

namespace winrt::WinUI3App::implementation
{
    struct ProgressBarControl : ProgressBarControlT<ProgressBarControl>
    {
        ProgressBarControl();

        int32_t MyProperty();
        void MyProperty(int32_t value);

    };
}

namespace winrt::WinUI3App::factory_implementation
{
    struct ProgressBarControl : ProgressBarControlT<ProgressBarControl, implementation::ProgressBarControl>
    {
    };
}
