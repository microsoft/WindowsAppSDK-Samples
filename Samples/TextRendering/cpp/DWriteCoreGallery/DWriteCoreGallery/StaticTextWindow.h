// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "ChildWindow.h"

/// <summary>
/// Child window used by various scenarios that displays static text from a
/// markdown resource.
/// </summary>
class StaticTextWindow : public ScrollableChildWindow
{
public:
    StaticTextWindow(HWND parentWindow, TextRenderer* textRenderer, std::vector<wil::com_ptr<IDWriteTextLayout4>>&& textLayouts = {});

    void SetTextLayouts(std::vector<wil::com_ptr<IDWriteTextLayout4>>&& textLayouts);

protected:
    void OnPaint(HDC hdc, RECT invalidRect) override;
    void OnSize() override;

private:
    std::vector<wil::com_ptr<IDWriteTextLayout4>> m_textLayouts;
};
