// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"
#include "VisualUtils.h"

/*static*/
void VisualUtils::FillWithColor(
    const winrt::SpriteVisual& visual,
    const winrt::Windows::UI::Color& color)
{
    visual.RelativeSizeAdjustment({ 1.0f, 1.0f });
    auto colorBrush = visual.Compositor().CreateColorBrush(color);
    visual.Brush(colorBrush);
}

/*static*/
void VisualUtils::LayoutAsInset(
    const winrt::Visual& visual, 
    float insetLeft,
    float insetTop,
    float insetRight,
    float insetBottom)
{
    auto relativeSize = visual.RelativeSizeAdjustment();
    if (relativeSize.x == 0.0f && relativeSize.y == 0.0f)
    {
        // If the relative size is not set, set it to 1.0f, 1.0f
        visual.RelativeSizeAdjustment({ 1.0f, 1.0f });
    }

    visual.Size({ -(insetLeft + insetRight), -(insetTop + insetBottom) });
    visual.Offset({ insetLeft, insetTop, 0.0f });
}

/*static*/
void VisualUtils::FillWithColor(
    const winrt::WUC::SpriteVisual& visual,
    const winrt::Windows::UI::Color& color)
{
    visual.RelativeSizeAdjustment({ 1.0f, 1.0f });
    auto colorBrush = visual.Compositor().CreateColorBrush(color);
    visual.Brush(colorBrush);
}

/*static*/
void VisualUtils::LayoutAsInset(
    const winrt::WUC::Visual& visual, 
    float insetLeft,
    float insetTop,
    float insetRight,
    float insetBottom)
{
    auto relativeSize = visual.RelativeSizeAdjustment();
    if (relativeSize.x == 0.0f && relativeSize.y == 0.0f)
    {
        // If the relative size is not set, set it to 1.0f, 1.0f
        visual.RelativeSizeAdjustment({ 1.0f, 1.0f });
    }

    visual.Size({ -(insetLeft + insetRight), -(insetTop + insetBottom) });
    visual.Offset({ insetLeft, insetTop, 0.0f });
}
