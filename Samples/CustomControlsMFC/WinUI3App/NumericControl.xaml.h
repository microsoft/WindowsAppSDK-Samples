#pragma once

#include "winrt/Microsoft.UI.Xaml.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
#include "winrt/Microsoft.UI.Xaml.Controls.Primitives.h"
#include "NumericControl.g.h"

namespace winrt::WinUI3App::implementation
{
    struct NumericControl : NumericControlT<NumericControl>
    {
        NumericControl();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void value(double val);
        double value();

        void setMinValue(double val);

        void setMaxValue(double val);
    };
}

namespace winrt::WinUI3App::factory_implementation
{
    struct NumericControl : NumericControlT<NumericControl, implementation::NumericControl>
    {
    };
}
