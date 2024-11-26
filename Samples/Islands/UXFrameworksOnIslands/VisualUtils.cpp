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
    float inset)
{
    LayoutAsInset(visual, inset, inset, inset, inset);
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
void VisualUtils::LayoutAsFillLeft(
    const winrt::Visual& visual,
    float percent)
{
    auto relativeSize = visual.RelativeSizeAdjustment();
    relativeSize.x = percent;

    // Don't clobber y if it's already set
    if (relativeSize.y == 0)
    {
        relativeSize.y = 1.0f;
    }

    visual.RelativeSizeAdjustment(relativeSize);
}

/*static*/
void VisualUtils::LayoutAsFillRight(
    const winrt::Visual& visual,
    float percent)
{
    auto relativeSize = visual.RelativeSizeAdjustment();
    relativeSize.x = percent;
    visual.RelativeSizeAdjustment(relativeSize);

    // Don't clobber y if it's already set
    if (relativeSize.y == 0)
    {
        relativeSize.y = 1.0f;
    }

    auto relativeOffset = visual.RelativeOffsetAdjustment();
    relativeOffset.x = 1.0f - percent;
    visual.RelativeOffsetAdjustment(relativeOffset);
}

/*static*/
void VisualUtils::LayoutAsFillTop(
    const winrt::Visual& visual,
    float percent)
{
    auto relativeSize = visual.RelativeSizeAdjustment();
    relativeSize.y = percent;

    // Don't clobber x if it's already set
    if (relativeSize.x == 0)
    {
        relativeSize.x = 1.0f;
    }

    visual.RelativeSizeAdjustment(relativeSize);
}

/*static*/
void VisualUtils::LayoutAsFillBottom(
    const winrt::Visual& visual,
    float percent)
{
    auto relativeSize = visual.RelativeSizeAdjustment();
    relativeSize.y = percent;

    // Don't clobber x if it's already set
    if (relativeSize.x == 0)
    {
        relativeSize.x = 1.0f;
    }

    visual.RelativeSizeAdjustment(relativeSize);

    auto relativeOffset = visual.RelativeOffsetAdjustment();
    relativeOffset.y = 1.0f - percent;
    visual.RelativeOffsetAdjustment(relativeOffset);
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
    float inset)
{
    LayoutAsInset(visual, inset, inset, inset, inset);
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

/*static*/
void VisualUtils::LayoutAsFillLeft(
    const winrt::WUC::Visual& visual,
    float percent)
{
    auto relativeSize = visual.RelativeSizeAdjustment();
    relativeSize.x = percent;

    // Don't clobber y if it's already set
    if (relativeSize.y == 0)
    {
        relativeSize.y = 1.0f;
    }

    visual.RelativeSizeAdjustment(relativeSize);
}

/*static*/
void VisualUtils::LayoutAsFillRight(
    const winrt::WUC::Visual& visual,
    float percent)
{
    auto relativeSize = visual.RelativeSizeAdjustment();
    relativeSize.x = percent;
    visual.RelativeSizeAdjustment(relativeSize);

    // Don't clobber y if it's already set
    if (relativeSize.y == 0)
    {
        relativeSize.y = 1.0f;
    }

    auto relativeOffset = visual.RelativeOffsetAdjustment();
    relativeOffset.x = 1.0f - percent;
    visual.RelativeOffsetAdjustment(relativeOffset);
}

/*static*/
void VisualUtils::LayoutAsFillTop(
    const winrt::WUC::Visual& visual,
    float percent)
{
    auto relativeSize = visual.RelativeSizeAdjustment();
    relativeSize.y = percent;

    // Don't clobber x if it's already set
    if (relativeSize.x == 0)
    {
        relativeSize.x = 1.0f;
    }

    visual.RelativeSizeAdjustment(relativeSize);
}

/*static*/
void VisualUtils::LayoutAsFillBottom(
    const winrt::WUC::Visual& visual,
    float percent)
{
    auto relativeSize = visual.RelativeSizeAdjustment();
    relativeSize.y = percent;

    // Don't clobber x if it's already set
    if (relativeSize.x == 0)
    {
        relativeSize.x = 1.0f;
    }

    visual.RelativeSizeAdjustment(relativeSize);

    auto relativeOffset = visual.RelativeOffsetAdjustment();
    relativeOffset.y = 1.0f - percent;
    visual.RelativeOffsetAdjustment(relativeOffset);
}
