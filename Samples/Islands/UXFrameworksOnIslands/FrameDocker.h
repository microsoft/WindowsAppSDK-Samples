// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

//
// Helper class used by frames to partition a rectangle (frame) into subrectangles for different visuals.
//
// If a nonzero rasterization scale is specified, the class guarantees that subrectangles are aligned on
// pixel boundaries. The implementation assumes that the origin of the coordinate space is pixel-aligned.
// 
// The class keeps track of the "available" rectangle not yet allocated to any visual. The Left, Top,
// Right, and Bottom method return the bounds of the available rectangle in logical units.
//
// The Inset* methods shrink the available rectangle to create white space.
//
// The DockTop* methods partition the available rectangle horizontally. The top partition is allocated
// to the specified visual and the bottom partition becomes the new available rectangle.
//
// The DockLeft* methods partition the available rectangle vertically. The left partition is allocated
// to the specified visual and the right partition becomes the new available retangle.
//
// The DockFill method allocates the remaining available rectange to the specified visual.
//
class FrameDocker
{
public:
    FrameDocker(winrt::Windows::Foundation::Numerics::float2 frameSize, float rasterizationScale) :
        m_right(frameSize.x),
        m_bottom(frameSize.y),
        m_rasterizationScale(rasterizationScale)
    {
    }

    // Getters for the available bounding rectangle.
    float Left() const noexcept { return m_left; }
    float Top() const noexcept { return m_top; }
    float Right() const noexcept { return m_right; }
    float Bottom() const noexcept { return m_bottom; }

    // Shrinks the available bounding rectangle by the specified amount.
    void Inset(float inset) noexcept
    {
        Inset(inset, inset, inset, inset);
    }
    void Inset(float left, float top, float right, float bottom) noexcept;
    void InsetLeft(float inset) noexcept;
    void InsetTop(float inset) noexcept;
    void InsetRight(float inset) noexcept;
    void InsetBottom(float inset) noexcept;

    // DockTop partitions the available rectangle horizontally, allocating
    // the top partition to the docked visual.
    winrt::Windows::Foundation::Rect DockTop(float height);

    template<class T>
    void DockTop(T& visual, float height)
    {
        SetVisualBounds(visual, DockTop(height));
    }

    template<class T>
    void DockTopRelative(T& visual, float relativeHeight)
    {
        DockTop(visual, relativeHeight * (m_bottom - m_top));
    }

    template<class T>
    void DockTopRelativeWithMargin(T& visual, float relativeHeight, float margin)
    {
        DockTop(visual, relativeHeight * (m_bottom - m_top - margin));
        InsetTop(margin);
    }

    // DockLeft partitions the available rectangle vertically, allocating
    // the left partition to the docked visual.
    winrt::Windows::Foundation::Rect DockLeft(float width);

    template<class T>
    void DockLeft(T& visual, float width)
    {
        SetVisualBounds(visual, DockLeft(width));
    }

    template<class T>
    void DockLeftRelative(T& visual, float relativeWidth)
    {
        DockLeft(visual, relativeWidth * (m_right - m_left));
    }

    template<class T>
    void DockLeftRelativeWithMargin(T& visual, float relativeWidth, float margin)
    {
        DockLeft(visual, relativeWidth * (m_right - m_left - margin));
        InsetLeft(margin);
    }

    // DockFill allocates the available rectangle to the specified visual.
    winrt::Windows::Foundation::Rect DockFill();

    template<class T>
    void DockFill(T& visual)
    {
        SetVisualBounds(visual, DockFill());
    }

private:
    template<class T>
    void SetVisualBounds(T& visual, winrt::Windows::Foundation::Rect const& rect)
    {
        visual.Offset({rect.X, rect.Y, 0.0f});
        visual.Size({rect.Width, rect.Height});
    }

    float RoundToPixel(float logicalCoord);

    // Bounding rectangle of the "available" space not yet allocated to any visual.
    float m_left = 0;
    float m_top = 0;
    float m_right = 0;
    float m_bottom = 0;

    // Rasterization scale, used for rounding logical coordinates to pixel boundaries.
    float m_rasterizationScale = 1;
};
