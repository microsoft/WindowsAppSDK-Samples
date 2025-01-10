// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

class VisualUtils
{
public:
    // Fills a sprite visual with a solid color brush.
    static void FillWithColor(
        const winrt::SpriteVisual& visual, 
        const winrt::Windows::UI::Color& color);

    // Lays out a visual to fill its container with a specified margin around it.
    template<class T>
    static void LayoutAsInset(
        const T& visual, 
        float inset)
    {
        LayoutAsInset(visual, inset, inset, inset, inset);
    }

    // Lays out a visual to fill its container with a specified margin around it.
    static void LayoutAsInset(
        const winrt::Visual& visual, 
        float insetLeft,
        float insetTop,
        float insetRight,
        float insetBottom);

    // Fills a sprite visual with a solid color brush.
    static void FillWithColor(
        const winrt::WUC::SpriteVisual& visual, 
        const winrt::Windows::UI::Color& color);

    // Lays out a visual to fill its container with a specified margin around it.
    static void LayoutAsInset(
        const winrt::WUC::Visual& visual, 
        float insetLeft,
        float insetTop,
        float insetRight,
        float insetBottom);
};

