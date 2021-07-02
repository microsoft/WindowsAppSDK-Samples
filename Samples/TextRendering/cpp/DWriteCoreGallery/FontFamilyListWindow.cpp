// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "Main.h"
#include "FontFamilyListWindow.h"
#include "Helpers.h"

namespace
{
    constexpr float g_fontSize = 16;
    constexpr float g_leftMargin = 10;
    constexpr float g_topMargin = 3;
    constexpr float g_bottomMargin = 3;

    const wchar_t g_familyName[] = L"Segoe UI";
    const DWRITE_FONT_AXIS_VALUE g_axisValues[] = { { DWRITE_FONT_AXIS_TAG_WEIGHT, 350 }, { DWRITE_FONT_AXIS_TAG_ITALIC, 0 } };
}

FontFamilyListWindow::FontFamilyListWindow(
    HWND parentWindow,
    TextRenderer* textRenderer,
    IDWriteFontCollection3* fontCollection,
    DWRITE_FONT_FAMILY_MODEL fontFamilyModel
) : ListWindow{ parentWindow, textRenderer },
    m_inputFontCollection{ fontCollection },
    m_fontFamilyModel{ fontFamilyModel }
{
    // Set the m_fontCollection member to the input font collection if one was specfied, or to
    // the system font collection if the input font collection is null.
    if (fontCollection != nullptr)
    {
        m_fontCollection = fontCollection;
    }
    else
    {
        THROW_IF_FAILED(g_factory->GetSystemFontCollection(false, fontFamilyModel, &m_fontCollection));
    }

    // Create a text format object for drawing the family names.
    m_textFormat = CreateTextFormat(g_familyName, g_fontSize, g_axisValues);

    // Disable word wrapping.
    THROW_IF_FAILED(m_textFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP));

    // Create a dummy text layout to compute the item height.
    auto textLayout = CreateTextLayout(m_textFormat.get(), std::span<wchar_t const>(L"A", 1));
    DWRITE_TEXT_METRICS1 metrics;
    THROW_IF_FAILED(textLayout->GetMetrics(&metrics));
    m_itemHeight = metrics.height + g_topMargin + g_bottomMargin;

    // Set trimming, so characters that don't fit will be replaced with an ellipsis.
    wil::com_ptr<IDWriteInlineObject> trimmingSign;
    THROW_IF_FAILED(g_factory->CreateEllipsisTrimmingSign(m_textFormat.get(), &trimmingSign));

    DWRITE_TRIMMING trimming = { DWRITE_TRIMMING_GRANULARITY_CHARACTER };
    THROW_IF_FAILED(m_textFormat->SetTrimming(&trimming, trimmingSign.get()));

    // Initialize the vector of family names.
    uint32_t familyCount = m_fontCollection->GetFontFamilyCount();
    m_familyNames.reserve(familyCount);

    for (uint32_t i = 0; i < familyCount; i++)
    {
        wil::com_ptr<IDWriteFontFamily2> fontFamily;
        THROW_IF_FAILED(m_fontCollection->GetFontFamily(i, &fontFamily));

        wil::com_ptr<IDWriteLocalizedStrings> familyNames;
        THROW_IF_FAILED(fontFamily->GetFamilyNames(&familyNames));

        m_familyNames.push_back(GetLocalString(familyNames.get()));
    }

    // Sort the vector of family names.
    auto LessIgnoreCase = [](std::wstring const& left, std::wstring const& right)
    {
        return CompareStringW(
            LOCALE_USER_DEFAULT, NORM_IGNORECASE, 
            left.c_str(), static_cast<int>(left.length()), 
            right.c_str(), static_cast<int>(right.length())
        ) == CSTR_LESS_THAN;
    };
    std::sort(m_familyNames.begin(), m_familyNames.end(), LessIgnoreCase);

    // Initialize the list.
    SetItemCount(familyCount);
}

void FontFamilyListWindow::DrawItem(TextRenderer* textRenderer, int itemIndex, bool isSelected)
{
    // Create the text layout object.
    auto textLayout = CreateTextLayout(m_textFormat.get(), m_familyNames[itemIndex]);

    // Set the maximum width, for trimming.
    float widthInDips = GetPixelWidth() / textRenderer->GetDpiScale();
    THROW_IF_FAILED(textLayout->SetMaxWidth(std::max(0.0f, widthInDips - g_leftMargin)));

    // Draw the text.
    THROW_IF_FAILED(textLayout->Draw(nullptr, textRenderer, g_leftMargin, g_topMargin));
}

int FontFamilyListWindow::GetItemPixelHeight(float dpiScale, int itemIndex)
{
    return static_cast<int>(ceilf(m_itemHeight * dpiScale));
}
