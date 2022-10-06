#pragma once

#include "winrt/Microsoft.UI.Xaml.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
#include "winrt/Microsoft.UI.Xaml.Controls.Primitives.h"
#include "ButtonControl.g.h"

namespace winrt::WinUI3App::implementation
{
  struct ButtonControl : ButtonControlT<ButtonControl>
  {
  
    ButtonControl();

    int32_t MyProperty();
    void MyProperty(int32_t value);

    void ButtonWidget_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
    
    void text(const winrt::hstring& str);

    void icon(const winrt::hstring& str);

    winrt::event_token clicked(Windows::Foundation::EventHandler<bool> const& handler);
    void clicked(winrt::event_token const& handler);
  private:
    winrt::event<Windows::Foundation::EventHandler<bool>> m_clicked;
  };
}

namespace winrt::WinUI3App::factory_implementation
{
  struct ButtonControl : ButtonControlT<ButtonControl, implementation::ButtonControl>
  {
  };
}
