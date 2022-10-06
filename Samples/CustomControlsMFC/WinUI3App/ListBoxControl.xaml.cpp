#include "pch.h"
#include "ListBoxControl.xaml.h"
#if __has_include("ListBoxControl.g.cpp")
#include "ListBoxControl.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::WinUI3App::implementation
{
    ListBoxControl::ListBoxControl()
    {
        InitializeComponent();
    }

    int32_t ListBoxControl::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void ListBoxControl::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void ListBoxControl::ListBoxWidget_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e)
    {
      m_changed(*this, true);
    }

    winrt::event_token ListBoxControl::changed(Windows::Foundation::EventHandler<bool> const& handler)
    {
      return m_changed.add(handler);
    }

    void ListBoxControl::changed(winrt::event_token const& handler)
    {
      m_changed.remove(handler);
    }

    void ListBoxControl::addItem(const winrt::hstring& item)
    {
      this->ListBoxWidget().Items().Append(box_value(item));
    }
}
