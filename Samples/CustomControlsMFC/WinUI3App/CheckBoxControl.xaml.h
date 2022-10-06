#pragma once

#include "winrt/Microsoft.UI.Xaml.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
#include "winrt/Microsoft.UI.Xaml.Controls.Primitives.h"
#include "CheckBoxControl.g.h"

namespace winrt::WinUI3App::implementation
{
  struct CheckBoxControl : CheckBoxControlT<CheckBoxControl>
  {
    CheckBoxControl();

    int32_t MyProperty();
    void MyProperty(int32_t value);

    void CheckBoxWidget_Checked(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
    void CheckBoxWidget_Unchecked(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
    
    void checked(bool checked);
    bool checked();

    winrt::event_token changed(Windows::Foundation::EventHandler<bool> const& handler);
    void changed(winrt::event_token const& handler);

  private:
    winrt::event<Windows::Foundation::EventHandler<bool>> m_changed;
  };
}

namespace winrt::WinUI3App::factory_implementation
{
  struct CheckBoxControl : CheckBoxControlT<CheckBoxControl, implementation::CheckBoxControl>
  {
  };
}
