#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "StatusButton.g.h"

namespace winrt::WinUI3App::implementation
{
    struct StatusButton : StatusButtonT<StatusButton>
    {
        StatusButton();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void Main_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
    };
}

namespace winrt::WinUI3App::factory_implementation
{
    struct StatusButton : StatusButtonT<StatusButton, implementation::StatusButton>
    {
    };
}
