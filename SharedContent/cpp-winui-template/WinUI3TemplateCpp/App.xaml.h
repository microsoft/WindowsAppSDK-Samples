#pragma once

#pragma push_macro("GetCurrentTime")
#undef GetCurrentTime

#include "App.xaml.g.h"

#pragma pop_macro("GetCurrentTime")

namespace winrt::WinUI3TemplateCpp::implementation
{
    struct App : AppT<App>
    {
        App();

        void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);
        
    private:
        winrt::Microsoft::UI::Xaml::Window window{ nullptr };
    };
}
