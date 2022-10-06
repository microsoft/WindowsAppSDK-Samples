#pragma once

#include "winrt/Microsoft.UI.Xaml.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
#include "winrt/Microsoft.UI.Xaml.Controls.Primitives.h"
#include "ComboBoxControl.g.h"
#include "IEnumWidgetBase.h"

namespace winrt::WinUI3App::implementation
{
  struct ComboBoxControl : ComboBoxControlT<ComboBoxControl>, public IEnumWidgetBase
  {
    ComboBoxControl();

    int32_t MyProperty();
    void MyProperty(int32_t value);

    void ComboBoxWidget_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e);
  
    virtual void text(const winrt::hstring& str) override;
    virtual winrt::hstring text() override;

    virtual int index() override;
    virtual void index(int i) override;

    virtual void addItem(const winrt::hstring& item) override;
    virtual void clearItems() override;

    virtual winrt::event_token changed(Windows::Foundation::EventHandler<bool> const& handler) override;
    virtual void changed(winrt::event_token const& handler) override;
  private:
    winrt::event<Windows::Foundation::EventHandler<bool>> m_changed;
  };
}

namespace winrt::WinUI3App::factory_implementation
{
  struct ComboBoxControl : ComboBoxControlT<ComboBoxControl, implementation::ComboBoxControl>
  {
  };
}
