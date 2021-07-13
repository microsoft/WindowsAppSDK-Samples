// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "Main.h"
#include "ListWindow.h"

void ListWindow::SetItemCount(int itemCount)
{
    m_itemCount = itemCount;

    int totalHeight = 0;
    float dpiScale = GetTextRenderer()->GetDpiScale();

    for (int itemIndex = 0; itemIndex < itemCount; itemIndex++)
    {
        totalHeight += GetItemPixelHeight(dpiScale, itemIndex);
    }

    SetPixelScrollTop(0);
    SetPixelScrollHeight(totalHeight);
    InvalidateRect(GetHandle(), nullptr, true);
}

void ListWindow::SetSelectedIndex(int itemIndex)
{
    SetSelectedIndex(itemIndex, nullptr, false);
}

void ListWindow::SetSelectedIndex(int itemIndex, _In_opt_ RECT const* itemRect, bool ensureVisible)
{
    if (itemIndex < 0 || itemIndex >= m_itemCount)
    {
        itemIndex = -1;
    }

    if (itemIndex != m_selectedIndex)
    {
        int oldIndex = m_selectedIndex;
        m_selectedIndex = itemIndex;

        if (oldIndex >= 0)
        {
            RECT rect = GetItemRect(oldIndex);
            InvalidateRect(GetHandle(), &rect, true);
        }

        if (itemIndex >= 0)
        {
            RECT rect = itemRect != nullptr ? *itemRect : GetItemRect(itemIndex);
            InvalidateRect(GetHandle(), &rect, true);

            if (ensureVisible)
            {
                int distance =
                    (rect.top < 0) ? rect.top :
                    (rect.bottom > GetPixelHeight()) ? rect.bottom - GetPixelHeight() :
                    0;

                if (distance != 0)
                {
                    SetPixelScrollTop(GetPixelScrollTop() + distance);
                    UpdateWindow(GetHandle());
                }
            }
        }

        SendMessageW(GetParent(GetHandle()), WM_COMMAND, MAKEWPARAM(/*controlId*/ 0, LBN_SELCHANGE), reinterpret_cast<LPARAM>(GetHandle()));
    }
}

RECT ListWindow::GetItemRect(int itemIndex)
{
    int top = -GetPixelScrollTop();
    float dpiScale = GetTextRenderer()->GetDpiScale();

    for (int i = 0; i < itemIndex; i++)
    {
        top += GetItemPixelHeight(dpiScale, i);
    }

    int bottom = top + GetItemPixelHeight(dpiScale, itemIndex);

    return RECT{ 0, top, GetPixelWidth(), bottom };
}

int ListWindow::ItemIndexFromY(int y, _Out_ RECT* itemRect)
{
    if (y >= 0 && y < GetPixelHeight())
    {
        int top = -GetPixelScrollTop();
        float dpiScale = GetTextRenderer()->GetDpiScale();

        for (int i = 0; i < m_itemCount; i++)
        {
            int bottom = top + GetItemPixelHeight(dpiScale, i);

            if (bottom > y)
            {
                *itemRect = { 0, top, GetPixelWidth(), bottom };
                return i;
            }

            top = bottom;
        }
    }

    *itemRect = {};
    return -1;
}

void ListWindow::OnFocus(bool haveFocus)
{
    m_haveFocus = haveFocus;
    InvalidateSelectedItem();
}

void ListWindow::InvalidateSelectedItem()
{
    if (m_selectedIndex >= 0)
    {
        RECT rect = GetItemRect(m_selectedIndex);
        InvalidateRect(GetHandle(), &rect, true);
    }
}

void ListWindow::OnPaint(HDC hdc, RECT invalidRect)
{
    // Resize the text renderer to the size of one item.
    auto textRenderer = GetTextRenderer();
    float dpiScale = textRenderer->GetDpiScale();
    int itemTop = -GetPixelScrollTop();

    for (int itemIndex = 0; itemIndex < m_itemCount; itemIndex++)
    {
        int itemPixelHeight = GetItemPixelHeight(dpiScale, itemIndex);
        textRenderer->Resize(SIZE{ GetPixelWidth(), itemPixelHeight });

        int itemBottom = itemTop + itemPixelHeight;

        if (itemTop < invalidRect.bottom && itemBottom > invalidRect.top)
        {
            bool isSelected = (itemIndex == m_selectedIndex);
            textRenderer->Clear(isSelected ? COLOR_HIGHLIGHT : COLOR_WINDOW);
            textRenderer->SetTextColor(isSelected ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT);

            DrawItem(textRenderer, itemIndex, isSelected);

            if (isSelected && m_haveFocus)
            {
                RECT itemRect{ 0, 0, GetPixelWidth(), itemPixelHeight };
                DrawFocusRect(textRenderer->GetRenderTarget()->GetMemoryDC(), &itemRect);
            }

            textRenderer->CopyTo(hdc, POINT{ 0, itemTop });
        }

        itemTop = itemBottom;
    }
}

bool ListWindow::OnLeftButtonDown(int x, int y)
{
    RECT itemRect;
    int itemIndex = ItemIndexFromY(y, &itemRect);
    if (itemIndex >= 0 && itemIndex != m_selectedIndex)
    {
        SetSelectedIndex(itemIndex, &itemRect, false);
    }
    SetFocus(GetHandle());
    return true;
}

bool ListWindow::OnKeyDown(uint32_t keyCode)
{
    switch (keyCode)
    {
    case VK_UP:
        if (m_selectedIndex > 0)
        {
            SetSelectedIndex(m_selectedIndex - 1, nullptr, true);
        }
        return true;

    case VK_DOWN:
        if (m_selectedIndex >= 0 && m_selectedIndex  < m_itemCount - 1)
        {
            SetSelectedIndex(m_selectedIndex + 1, nullptr, true);
        }
        return true;

    case VK_HOME:
        if (m_selectedIndex > 0)
        {
            SetSelectedIndex(0, nullptr, true);
        }
        return true;

    case VK_END:
        if (m_selectedIndex >= 0 && m_selectedIndex < m_itemCount - 1)
        {
            SetSelectedIndex(m_itemCount - 1, nullptr, true);
        }
        return true;

    case VK_PRIOR:
        if (m_selectedIndex > 0)
        {
            int count = ItemCountPerPage(m_selectedIndex - 1, -1, -1);
            SetSelectedIndex(m_selectedIndex - count, nullptr, true);
        }
        return true;

    case VK_NEXT:
        if (m_selectedIndex >= 0 && m_selectedIndex < m_itemCount - 1)
        {
            int count = ItemCountPerPage(m_selectedIndex + 1, m_itemCount, 1);
            SetSelectedIndex(m_selectedIndex + count, nullptr, true);
        }
        return true;
    }
    return false;
}

int ListWindow::ItemCountPerPage(int startIndex, int endIndex, int increment)
{
    float dpiScale = GetTextRenderer()->GetDpiScale();

    int const pageHeight = GetPixelHeight();

    int itemCount = 0;
    int totalItemHeight = 0;

    for (int i = startIndex; i != endIndex; i += increment)
    {
        totalItemHeight += GetItemPixelHeight(dpiScale, i);
        if (totalItemHeight >= pageHeight)
        {
            break;
        }

        itemCount++;
    }

    return std::max(itemCount, 1);
}
