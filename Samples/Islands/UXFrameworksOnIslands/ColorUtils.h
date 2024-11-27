// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

class ColorUtils
{
public:
    // 'FromHexValue(0xaabbccdd)' is the equivalent of in Xaml or other frameworks '#aabbccdd'
    static inline winrt::Windows::UI::Color FromHexValue(uint32_t hexValue)
    {
        return winrt::Windows::UI::ColorHelper::FromArgb(
            (hexValue >> 24) & 0xFF,
            (hexValue >> 16) & 0xFF,
            (hexValue >> 8) & 0xFF,
            hexValue & 0xFF);
    }

    static inline winrt::Windows::UI::Color LightBlue()
    {
        return FromHexValue(0xffc4d9ee);
    }

    static inline winrt::Windows::UI::Color LightBrown()
    {
        return FromHexValue(0xffe6d6b5);
    }

    static inline winrt::Windows::UI::Color LightGreen()
    {
        return FromHexValue(0xffccf0db);
    }

    static inline winrt::Windows::UI::Color DarkGreen()
    {
        return FromHexValue(0xff88ddab);
    }

    static inline winrt::Windows::UI::Color LightPink()
    {
        return FromHexValue(0xffe6c8c8);
    }

    static inline winrt::Windows::UI::Color DarkPink()
    {
        return FromHexValue(0xffdcb2b2);
    }

    static inline winrt::Windows::UI::Color LightYellow()
    {
        return FromHexValue(0xfff9f5e2);
    }

    static inline winrt::Windows::UI::Color Black()
    {
        return FromHexValue(0xff000000);
    }
};
