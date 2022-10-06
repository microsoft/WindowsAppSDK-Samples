#pragma once

#include "winrt/Microsoft.UI.Xaml.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
#include "winrt/Microsoft.UI.Xaml.Controls.Primitives.h"
#include "ListBoxControl.g.h"
#include "IEnumWidgetBase.h"

namespace winrt::WinUI3App::implementation
{
    struct ListBoxControl : ListBoxControlT<ListBoxControl>//, public IEnumWidgetBase
    {
        ListBoxControl();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void ListBoxWidget_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e);
        
        winrt::event_token changed(Windows::Foundation::EventHandler<bool> const& handler);
        void changed(winrt::event_token const& handler);

        virtual void addItem(const winrt::hstring& item) ;

    private:
      winrt::event<Windows::Foundation::EventHandler<bool>> m_changed;
    };
}

namespace winrt::WinUI3App::factory_implementation
{
    struct ListBoxControl : ListBoxControlT<ListBoxControl, implementation::ListBoxControl>
    {
    };
}
