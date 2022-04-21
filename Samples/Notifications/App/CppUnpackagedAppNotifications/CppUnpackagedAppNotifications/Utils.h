#pragma once
#include <winrt/Microsoft.Windows.AppNotifications.h>

class Utils
{
public:
    static std::wstring GetFullPathToAsset(std::wstring const& assetName);
};

