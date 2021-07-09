// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "Main.h"
#include "Helpers.h"
#include "SplitWindow.h"

namespace
{
    constexpr float g_splitterWidth = 7;

    constexpr float g_faceNameFontSize = 24;
}

SplitWindow::SplitWindow(HWND parentWindow, TextRenderer* textRenderer, float leftWidth) :
    ChildWindow{ parentWindow, textRenderer },
    m_leftWidth{ leftWidth }
{
}

void SplitWindow::OnSize()
{
    if (m_leftPane != nullptr && m_rightPane != nullptr)
    {
        float scale = GetTextRenderer()->GetDpiScale();

        int leftPaneWidth = std::min(GetPixelWidth(), static_cast<int>(m_leftWidth * scale));
        int rightPaneWidth = std::max(0, GetPixelWidth() - leftPaneWidth);

        m_leftPane->Move(0, 0, leftPaneWidth, GetPixelHeight());
        m_rightPane->Move(leftPaneWidth, 0, rightPaneWidth, GetPixelHeight());
    }
}

void SplitWindow::Initialize(std::unique_ptr<ChildWindow>&& leftPane, std::unique_ptr<ChildWindow>&& rightPane)
{
    m_leftPane = std::move(leftPane);
    m_rightPane = std::move(rightPane);
}
