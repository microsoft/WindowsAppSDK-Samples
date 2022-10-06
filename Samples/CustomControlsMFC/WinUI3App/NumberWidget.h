#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "NumberWidget.g.h"

namespace winrt::WinUI3App::implementation
{
    struct NumberWidget : NumberWidgetT<NumberWidget>
    {
        NumberWidget();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void Down_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void Up_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void NumberBoxWidget_ValueChanged(winrt::Microsoft::UI::Xaml::Controls::NumberBox const& sender, winrt::Microsoft::UI::Xaml::Controls::NumberBoxValueChangedEventArgs const& args);
    };
}

namespace winrt::WinUI3App::factory_implementation
{
    struct NumberWidget : NumberWidgetT<NumberWidget, implementation::NumberWidget>
    {
    };
}
