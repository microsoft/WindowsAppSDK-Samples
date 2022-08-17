#pragma once

#include "App.xaml.g.h"

namespace winrt::CppWinUiDesktopInstancing::implementation
{
    struct App : AppT<App>
    {
        const DWORD c_maxNumberOfInstances{ 2 };
        App();

        void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);

    private:
        winrt::Microsoft::UI::Xaml::Window window{ nullptr };
    };
}
