#pragma once

#include "winrt/Microsoft.UI.Xaml.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
#include "winrt/Microsoft.UI.Xaml.Controls.Primitives.h"
#include "SliderControl.g.h"

namespace winrt::WinUI3App::implementation
{
    struct SliderControl : SliderControlT<SliderControl>
    {
        SliderControl();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void SliderWidget_ValueChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& e);
        
        void value(int str);
        int value();

        void setMinValue(int str);

        void setMaxValue(int str);

        winrt::event_token changed(Windows::Foundation::EventHandler<bool> const& handler);
        void changed(winrt::event_token const& handler);
    private:
      winrt::event<Windows::Foundation::EventHandler<bool>> m_changed;
    };
}

namespace winrt::WinUI3App::factory_implementation
{
    struct SliderControl : SliderControlT<SliderControl, implementation::SliderControl>
    {
    };
}
