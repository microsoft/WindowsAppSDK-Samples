// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "Main.h"
#include "Helpers.h"
#include "StaticTextWindow.h"

namespace
{
    constexpr float g_leftMargin = 20;
    constexpr float g_rightMargin = 20;
    constexpr float g_topMargin = 10;
    constexpr float g_bottomMargin = 20;
    constexpr float g_minColumnWidth = 96;
    constexpr float g_paragraphGap = 13;
}

StaticTextWindow::StaticTextWindow(HWND parentWindow, TextRenderer* textRenderer, std::vector<wil::com_ptr<IDWriteTextLayout4>>&& textLayouts) :
    ScrollableChildWindow{ parentWindow, textRenderer },
    m_textLayouts(std::move(textLayouts))
{
}

void StaticTextWindow::SetTextLayouts(std::vector<wil::com_ptr<IDWriteTextLayout4>>&& textLayouts)
{
    m_textLayouts = std::move(textLayouts);
    SetPixelScrollTop(0);
    OnSize();
    InvalidateRect(GetHandle(), nullptr, true);
}

void StaticTextWindow::OnPaint(HDC hdc, RECT invalidRect)
{
    auto g = GetTextRenderer();

    // Get the scale factor to convert from DIPs (1/96 inch) to pixels.
    float scale = g->GetDpiScale();

    // Resize the render target to the size of invalid rect (in pixels).
    int targetPixelWidth = invalidRect.right - invalidRect.left;
    int targetPixelHeight = invalidRect.bottom - invalidRect.top;
    g->Resize({ targetPixelWidth, targetPixelHeight });

    // Compute the target height in DIPs.
    const float targetHeight = targetPixelHeight / scale;

    // Compute the offset in DIPs from the top-left corner of the invalid
    // rect to the top-left corner of the first text layout.
    float targetX = g_leftMargin - (invalidRect.left / scale);
    float targetY = g_topMargin - ((invalidRect.top + GetPixelScrollTop()) / scale);

    // Clear the background and set the text color.
    g->Clear(COLOR_WINDOW);
    g->SetTextColor(COLOR_WINDOWTEXT);

    // Draw the text layouts to the off-screen render target.
    for (auto& textLayout : m_textLayouts)
    {
        DWRITE_TEXT_METRICS1 metrics;
        THROW_IF_FAILED(textLayout->GetMetrics(&metrics));

        if (targetY + metrics.height >= 0)
        {
            THROW_IF_FAILED(textLayout->Draw(nullptr, GetTextRenderer(), targetX, targetY));
        }

        targetY += metrics.height + g_paragraphGap;

        if (targetY > targetHeight)
        {
            break;
        }
    }

    // Copy the render target to the window HDC.
    GetTextRenderer()->CopyTo(hdc, { invalidRect.left, invalidRect.top });
}

void StaticTextWindow::OnSize()
{
    float scale = GetTextRenderer()->GetDpiScale();
    float clientWidthInDips = GetPixelWidth() / scale;

    float columnWidth = std::max(
        clientWidthInDips - (g_leftMargin + g_rightMargin),
        g_minColumnWidth
    );

    float height = g_topMargin;

    for (auto& textLayout : m_textLayouts)
    {
        THROW_IF_FAILED(textLayout->SetMaxWidth(columnWidth));

        DWRITE_TEXT_METRICS1 metrics;
        THROW_IF_FAILED(textLayout->GetMetrics(&metrics));

        height += metrics.height + g_paragraphGap;
    }

    height += g_bottomMargin - g_paragraphGap;

    SetPixelScrollHeight(static_cast<int>(ceilf(height * scale)));
}
