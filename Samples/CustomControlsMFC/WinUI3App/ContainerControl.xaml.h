#pragma once

#include "winrt/Microsoft.UI.Xaml.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
#include "winrt/Microsoft.UI.Xaml.Controls.Primitives.h"
#include "ContainerControl.g.h"

namespace winrt::WinUI3App::implementation
{
    struct ContainerControl : ContainerControlT<ContainerControl>
    {
        ContainerControl();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void myButton_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
    };
}

namespace winrt::WinUI3App::factory_implementation
{
    struct ContainerControl : ContainerControlT<ContainerControl, implementation::ContainerControl>
    {
    };
}
