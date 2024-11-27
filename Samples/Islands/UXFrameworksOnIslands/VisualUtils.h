// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

class VisualUtils
{
public:
    static void FillWithColor(
        const winrt::SpriteVisual& visual, 
        const winrt::Windows::UI::Color& color);

    // Layout utils. These are useful for partitioning a scene, and will stack with each other.
    // For example if you want a visual to be inset by 10px but also float in the top right quadrant,
    // relative to its parent, you can call:
    //      LayoutAsInset(visual, 10.0f);
    //      LayoutAsFillRight(visual, 0.5f);
    //      LayoutAsFillTop(visual, 0.5f);
    static void LayoutAsInset(
        const winrt::Visual& visual, 
        float inset);

    static void LayoutAsInset(
        const winrt::Visual& visual, 
        float insetLeft,
        float insetTop,
        float insetRight,
        float insetBottom);

    static void LayoutAsFillLeft(
        const winrt::Visual& visual, 
        float percent);

    static void LayoutAsFillRight(
        const winrt::Visual& visual, 
        float percent);

    static void LayoutAsFillTop(
        const winrt::Visual& visual, 
        float percent);

    static void LayoutAsFillBottom(
        const winrt::Visual& visual, 
        float percent);

    static void FillWithColor(
        const winrt::WUC::SpriteVisual& visual, 
        const winrt::Windows::UI::Color& color);

    // Layout utils. These are useful for partitioning a scene, and will stack with each other.
    // For example if you want a visual to be inset by 10px but also float in the top right quadrant,
    // relative to its parent, you can call:
    //      LayoutAsInset(visual, 10.0f);
    //      LayoutAsFillRight(visual, 0.5f);
    //      LayoutAsFillTop(visual, 0.5f);
    static void LayoutAsInset(
        const winrt::WUC::Visual& visual, 
        float inset);

    static void LayoutAsInset(
        const winrt::WUC::Visual& visual, 
        float insetLeft,
        float insetTop,
        float insetRight,
        float insetBottom);

    static void LayoutAsFillLeft(
        const winrt::WUC::Visual& visual, 
        float percent);

    static void LayoutAsFillRight(
        const winrt::WUC::Visual& visual, 
        float percent);

    static void LayoutAsFillTop(
        const winrt::WUC::Visual& visual, 
        float percent);

    static void LayoutAsFillBottom(
        const winrt::WUC::Visual& visual, 
        float percent);
};
