// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "App.xaml.g.h"
#include "pch.h"

namespace winrt::CppUnpackagedAppNotifications::implementation
{
    struct App : AppT<App>
    {
        App();

        static std::wstring GetFullPathToExe();
        static std::wstring GetFullPathToAsset(std::wstring const& assetName);
        static void ToForeground();

        void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);

    private:
        Microsoft::UI::Xaml::Window window{ nullptr };
    };
}
