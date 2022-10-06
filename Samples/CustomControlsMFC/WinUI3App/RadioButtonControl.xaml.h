#pragma once

#include "winrt/Microsoft.UI.Xaml.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
#include "winrt/Microsoft.UI.Xaml.Controls.Primitives.h"
#include "RadioButtonControl.g.h"

namespace winrt::WinUI3App::implementation
{
    struct RadioButtonControl : RadioButtonControlT<RadioButtonControl>
    {
        RadioButtonControl();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void RadioButtonWidget_Checked(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void RadioButtonWidget_Unchecked(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void RadioButtonWidget_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

        winrt::hstring text();

        void isChecked(bool check);
        bool isChecked();

        winrt::event_token clicked(Windows::Foundation::EventHandler<bool> const& handler);
        void clicked(winrt::event_token const& handler);
        winrt::event_token changed(Windows::Foundation::EventHandler<bool> const& handler);
        void changed(winrt::event_token const& handler);
        winrt::event_token checked(Windows::Foundation::EventHandler<bool> const& handler);
        void checked(winrt::event_token const& handler);
    private:
      winrt::event<Windows::Foundation::EventHandler<bool>> m_clicked;
      winrt::event<Windows::Foundation::EventHandler<bool>> m_changed;
      winrt::event<Windows::Foundation::EventHandler<bool>> m_checked;
    };
}

namespace winrt::WinUI3App::factory_implementation
{
    struct RadioButtonControl : RadioButtonControlT<RadioButtonControl, implementation::RadioButtonControl>
    {
    };
}
