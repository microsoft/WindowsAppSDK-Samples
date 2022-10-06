#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "DropdownButton.g.h"

namespace winrt::WinUI3App::implementation
{
    struct DropdownButton : DropdownButtonT<DropdownButton>
    {
        DropdownButton();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void Main_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e);
    };
}

namespace winrt::WinUI3App::factory_implementation
{
    struct DropdownButton : DropdownButtonT<DropdownButton, implementation::DropdownButton>
    {
    };
}
