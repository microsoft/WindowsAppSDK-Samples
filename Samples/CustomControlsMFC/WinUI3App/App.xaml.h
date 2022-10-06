#pragma once

#include "App.xaml.g.h"
#include "IEnumWidgetBase.h"

namespace winrt::WinUI3App::implementation
{
    struct App : AppT<App>
    {
        App();

        void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);

    private:
        winrt::Microsoft::UI::Xaml::Window window{ nullptr };
    };
}
