// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "ChildWindow.h"

class ListWindow : public ScrollableChildWindow
{
public:
    ListWindow(HWND parentWindow, TextRenderer* textRenderer) : ScrollableChildWindow{ parentWindow, textRenderer }
    {
    }

    void SetSelectedIndex(int itemIndex);
    int GetSelectedIndex() const noexcept { return m_selectedIndex; }

    int GetItemCount() const noexcept { return m_itemCount; }

protected:
    virtual void DrawItem(TextRenderer* textRenderer, int itemIndex, bool isSelected) = 0;
    virtual int GetItemPixelHeight(float dpiScale, int itemIndex) = 0;

    void SetItemCount(int itemCount);

    void OnPaint(HDC hdc, RECT invalidRect) override;
    bool OnLeftButtonDown(int x, int y) override;
    bool OnKeyDown(uint32_t keyCode) override;
    void OnFocus(bool haveFocus) override;

private:
    RECT GetItemRect(int itemIndex);
    void InvalidateSelectedItem();
    int ItemIndexFromY(int y, _Out_ RECT* itemRect);
    void SetSelectedIndex(int itemIndex, _In_opt_ RECT const* itemRect, bool ensureVisible);
    int ItemCountPerPage(int startIndex, int endIndex, int increment);

    int m_itemCount = 0;
    int m_selectedIndex = -1;
    bool m_haveFocus = false;
};
