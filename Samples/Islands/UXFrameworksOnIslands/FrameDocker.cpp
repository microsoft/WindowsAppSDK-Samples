// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"
#include "FrameDocker.h"

// Shrinks the bounding rectangle by the specified amount on all sides.
void FrameDocker::Inset(float left, float top, float right, float bottom) noexcept
{
    InsetLeft(left);
    InsetTop(top);
    InsetRight(right);
    InsetBottom(bottom);
}

void FrameDocker::InsetLeft(float inset) noexcept
{
    m_left = RoundToPixel(m_left + inset);
    m_left = std::min(m_left, m_right);
}

void FrameDocker::InsetTop(float inset) noexcept
{
    m_top = RoundToPixel(m_top + inset);
    m_top = std::min(m_top, m_bottom);
}

void FrameDocker::InsetRight(float inset) noexcept
{
    m_right = RoundToPixel(m_right - inset);
    m_right = std::max(m_left, m_right);
}

void FrameDocker::InsetBottom(float inset) noexcept
{
    m_bottom = RoundToPixel(m_bottom - inset);
    m_bottom = std::max(m_top, m_bottom);
}

winrt::Windows::Foundation::Rect FrameDocker::DockTop(float height)
{
    // Compute the Y coordinate of the boundary between the docked item
    // (above) and the remaining available space (below).
    float y = m_top + std::max(height, 0.0f);

    // Round to a pixel boundary, and clamp to the bounding rectange.
    y = RoundToPixel(y);
    y = std::min(y, m_bottom);

    // Compute the bounding rectange of the docked item.
    winrt::Windows::Foundation::Rect dockedRect{
        /*X*/ m_left,
        /*Y*/ m_top,
        /*Width*/ m_right - m_left,
        /*Height*/ y - m_top
    };

    // The new available bounds is below y.
    m_top = y;

    return dockedRect;
}

winrt::Windows::Foundation::Rect FrameDocker::DockLeft(float width)
{
    // Compute the X coordinate of the boundary between the docked item
    // (left) and the remaining available space (right).
    float x = m_left + std::max(width, 0.0f);

    // Round to a pixel boundary, and clamp to the bounding rectange.
    x = RoundToPixel(x);
    x = std::min(x, m_right);

    // Compute the bounding rectange of the docked item.
    winrt::Windows::Foundation::Rect dockedRect{
        /*X*/ m_left,
        /*Y*/ m_top,
        /*Width*/ x - m_left,
        /*Height*/ m_bottom - m_top
    };

    // The new available bounds is to the right of x.
    m_left = x;

    return dockedRect;
}

winrt::Windows::Foundation::Rect FrameDocker::DockFill()
{
    return winrt::Windows::Foundation::Rect{
        /*X*/ m_left,
        /*Y*/ m_top,
        /*Width*/ m_right - m_left,
        /*Height*/ m_bottom - m_top
    };
}

float FrameDocker::RoundToPixel(float logicalCoord)
{
    if (m_rasterizationScale != 0)
    {
        // Convert to device units, round, and convert back.
        float pixelCoord = logicalCoord * m_rasterizationScale;
        return floorf(pixelCoord + 0.5f) / m_rasterizationScale;
    }
    else
    {
        // No rasterization scale specified: do not round.
        return logicalCoord;
    }
}
