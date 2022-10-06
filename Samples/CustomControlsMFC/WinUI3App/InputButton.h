#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "InputButton.g.h"

namespace winrt::WinUI3App::implementation
{
    struct InputButton : InputButtonT<InputButton>
    {
        InputButton();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void ButtonInput_TextChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::TextChangedEventArgs const& e);
    };
}

namespace winrt::WinUI3App::factory_implementation
{
    struct InputButton : InputButtonT<InputButton, implementation::InputButton>
    {
    };
}
