// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "Main.h"
#include "Helpers.h"
#include "Resource.h"
#include "FontFaceListWindow.h"

namespace
{
    constexpr float g_leftMargin = 20;
    constexpr float g_topMargin = 15;
    constexpr float g_bottomMargin = 10;
    constexpr float g_gutter = 20;
    constexpr float g_rowGap = 10;
    constexpr float g_rowPadding = 2;

    constexpr float g_faceNameFontSize = 30.0f;
    constexpr float g_rowFontSize = 12.0f;

    constexpr wchar_t g_familyName[] = L"Segoe UI";

    const DWRITE_FONT_AXIS_VALUE g_labelAxisValues[] =
    {
        { DWRITE_FONT_AXIS_TAG_WEIGHT, 600 },
        { DWRITE_FONT_AXIS_TAG_ITALIC, 0 }
    };

    const DWRITE_FONT_AXIS_VALUE g_valueAxisValues[] =
    {
        { DWRITE_FONT_AXIS_TAG_WEIGHT, 400 },
        { DWRITE_FONT_AXIS_TAG_ITALIC, 0 }
    };
}

FontFaceListWindow::FontFaceListWindow(HWND parentWindow, TextRenderer* textRenderer) :
    ListWindow{ parentWindow, textRenderer }
{
    // Create text format objects for displaying property labels and values.
    m_labelTextFormat = CreateTextFormat(g_familyName, g_rowFontSize, g_labelAxisValues);
    THROW_IF_FAILED(m_labelTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP));

    m_valueTextFormat = CreateTextFormat(g_familyName, g_rowFontSize, g_valueAxisValues);
    THROW_IF_FAILED(m_valueTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP));

    // Create text layout objects for each of the labels.
    // This will also initialize the m_maxLabelWidth and m_rowHeight members.
    m_fileNameLabel = CreateLabel(IDS_FILE_NAME);
    m_faceIndexLabel = CreateLabel(IDS_FACE_INDEX);
    m_typoFamilyNameLabel = CreateLabel(IDS_TYPO_FAMILY_NAME);
    m_wssFamilyNameLabel = CreateLabel(IDS_WSS_FAMILY_NAME);
    m_gdiFamilyNameLabel = CreateLabel(IDS_GDI_FAMILY_NAME);
    m_fullNameLabel = CreateLabel(IDS_FULL_NAME);
    m_postNameLabel = CreateLabel(IDS_POST_NAME);

    m_rowHeight += g_rowPadding;
}

wil::com_ptr<IDWriteTextLayout4> FontFaceListWindow::CreateLabel(uint32_t stringId)
{
    wchar_t buffer[256];
    int length = LoadStringW(g_hInstance, stringId, buffer, ARRAYSIZE(buffer));
    THROW_LAST_ERROR_IF(length <= 0);

    auto textLayout = CreateTextLayout(m_labelTextFormat.get(), buffer);

    DWRITE_TEXT_METRICS1 metrics;
    THROW_IF_FAILED(textLayout->GetMetrics(&metrics));

    if (metrics.width > m_maxLabelWidth)
    {
        m_maxLabelWidth = metrics.width;
    }

    if (metrics.height > m_rowHeight)
    {
        m_rowHeight = metrics.height;
    }

    return textLayout;
}

void FontFaceListWindow::SetFontFamily(
    _In_ IDWriteFontFamily2* fontFamily,
    DWRITE_FONT_FAMILY_MODEL fontFamilyModel,
    _In_opt_ IDWriteFontCollection3* customFontCollection
    )
{
    // Create a new vector of font items, one for each font.
    uint32_t fontCount = fontFamily->GetFontCount();
    std::vector<FontItem> fontItems(fontCount);

    // Initialize each font item.
    for (uint32_t fontIndex = 0; fontIndex < fontCount; fontIndex++)
    {
        auto& item = fontItems[fontIndex];

        // Create a font face for this font.
        wil::com_ptr<IDWriteFontFaceReference> fontFaceReference;
        THROW_IF_FAILED(fontFamily->GetFontFaceReference(fontIndex, &fontFaceReference));
        wil::com_ptr<IDWriteFontFace3> fontFace3;
        THROW_IF_FAILED(fontFaceReference->CreateFontFace(&fontFace3));
        auto fontFace = fontFace3.query<IDWriteFontFace6>();

        // Initialize the file name.
        item.m_fileName = GetFontFilePath(fontFace.get());

        // Get the face index only if it's an OpenType collection.
        if (fontFace->GetType() == DWRITE_FONT_FACE_TYPE_OPENTYPE_COLLECTION)
        {
            item.m_faceIndex = fontFaceReference->GetFontFaceIndex();
        }

        item.m_typoFamilyName = GetFamilyName(fontFace.get(), DWRITE_FONT_FAMILY_MODEL_TYPOGRAPHIC);
        item.m_wssFamilyName = GetFamilyName(fontFace.get(), DWRITE_FONT_FAMILY_MODEL_WEIGHT_STRETCH_STYLE);
        item.m_gdiFamilyName = GetInformationalString(fontFace.get(), DWRITE_INFORMATIONAL_STRING_WIN32_FAMILY_NAMES);
        item.m_fullName = GetInformationalString(fontFace.get(), DWRITE_INFORMATIONAL_STRING_FULL_NAME);
        item.m_postName = GetInformationalString(fontFace.get(), DWRITE_INFORMATIONAL_STRING_POSTSCRIPT_NAME);

        // Initialize the vector of axes.
        auto axisValues = GetFontAxisValues(fontFace.get());
        auto axisRanges = GetFontAxisRanges(fontFace.get());
        THROW_HR_IF(E_UNEXPECTED, axisRanges.size() != axisValues.size());
        item.m_axes.resize(axisValues.size());
        for (size_t i = 0; i < axisValues.size(); i++)
        {
            AxisInfo& info = item.m_axes[i];
            info.axisTag = axisValues[i].axisTag;
            info.value = axisValues[i].value;
            info.minValue = axisRanges[i].minValue;
            info.maxValue = axisRanges[i].maxValue;
        }

        // Create a text layout object for the face name, styled to match the font.
        auto itemFormat = CreateTextFormat(item.m_typoFamilyName.c_str(), g_faceNameFontSize, axisValues, customFontCollection);
        THROW_IF_FAILED(itemFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP));
        item.m_faceNameTextLayout = CreateTextLayout(itemFormat.get(), GetFaceName(fontFace.get(), fontFamilyModel));

        // Initialize the face name height.
        DWRITE_TEXT_METRICS1 metrics;
        THROW_IF_FAILED(item.m_faceNameTextLayout->GetMetrics(&metrics));
        item.m_faceNameHeight = metrics.height;

        // Begin computing the height, starting with the top margin and face name.
        float height =
            g_topMargin +
            item.m_faceNameHeight;

        // Add space for the names and axis values.
        height +=
            g_rowGap +
            m_rowHeight * 5 + // font names
            g_rowGap +
            (item.m_axes.size() * m_rowHeight); // axis values

        // Add space for the file name and/or face index if present.
        if (!item.m_fileName.empty() || item.m_faceIndex >= 0)
        {
            height += g_rowGap;

            if (!item.m_fileName.empty())
            {
                height += m_rowHeight;
            }

            if (item.m_faceIndex >= 0)
            {
                height += m_rowHeight;
            }
        }

        height += g_bottomMargin;

        item.m_itemHeight = height;
    }

    m_fontItems = std::move(fontItems);
    ListWindow::SetItemCount(fontCount);
}

void FontFaceListWindow::DrawItem(TextRenderer* textRenderer, int itemIndex, bool isSelected)
{
    auto& item = m_fontItems[itemIndex];

    // Drawing coordinates.
    constexpr float leftX = g_leftMargin;
    const float rightX = g_leftMargin + g_gutter + m_maxLabelWidth;
    float y = g_topMargin;

    // Character buffer for numeric values.
    wchar_t valueBuffer[80];

    // Helper for drawing labels.
    auto DrawLabel = [&](IDWriteTextLayout4* textLayout)
    {
        THROW_IF_FAILED(textLayout->Draw(nullptr, textRenderer, leftX, y));
    };

    // Helper for drawing values.
    auto DrawValue = [&](_In_z_ wchar_t const* value)
    {
        auto textLayout = CreateTextLayout(m_valueTextFormat.get(), value);
        THROW_IF_FAILED(textLayout->Draw(nullptr, textRenderer, rightX, y));
    };

    // Draw the face name.
    DrawLabel(item.m_faceNameTextLayout.get());
    y += item.m_faceNameHeight;

    // Add a gap before the names.
    y += g_rowGap;

    // Draw the typographic family name.
    DrawLabel(m_typoFamilyNameLabel.get());
    DrawValue(item.m_typoFamilyName.c_str());
    y += m_rowHeight;

    // Draw the weight-stretch-style family name.
    DrawLabel(m_wssFamilyNameLabel.get());
    DrawValue(item.m_wssFamilyName.c_str());
    y += m_rowHeight;

    // Draw the GDI family name.
    DrawLabel(m_gdiFamilyNameLabel.get());
    DrawValue(item.m_gdiFamilyName.c_str());
    y += m_rowHeight;

    // Draw the full name.
    DrawLabel(m_fullNameLabel.get());
    DrawValue(item.m_fullName.c_str());
    y += m_rowHeight;

    // Draw the Postscript name.
    DrawLabel(m_postNameLabel.get());
    DrawValue(item.m_postName.c_str());
    y += m_rowHeight;

    // Add a gap before the axis values.
    y += g_rowGap;

    for (AxisInfo const& axis : item.m_axes)
    {
        // Draw the axis tag.
        uint32_t tagValue = axis.axisTag;
        wchar_t tagString[] = { tagValue & 0xFF, (tagValue >> 8) & 0xFF, (tagValue >> 16) & 0xFF, (tagValue >> 24) & 0xFF, 0 };
        auto textLayout = CreateTextLayout(m_labelTextFormat.get(), tagString);
        DrawLabel(textLayout.get());

        // Create the value string.
        if (axis.minValue < axis.maxValue)
        {
            swprintf_s(valueBuffer, L"%0.2f (%0.2f - %0.2f)", axis.value, axis.minValue, axis.maxValue);
        }
        else
        {
            swprintf_s(valueBuffer, L"%0.2f", axis.value);
        }

        DrawValue(valueBuffer);
        y += m_rowHeight;
    }

    // Draw the file name and/or face index if present.
    if (!item.m_fileName.empty() || item.m_faceIndex >= 0)
    {
        // Add a gap before the file name and/or face index.
        y += g_rowGap;

        // Draw the file name if it exists.
        if (!item.m_fileName.empty())
        {
            DrawLabel(m_fileNameLabel.get());
            DrawValue(item.m_fileName.c_str());
            y += m_rowHeight;
        }

        // Draw the face index if there is one.
        if (item.m_faceIndex >= 0)
        {
            DrawLabel(m_faceIndexLabel.get());

            swprintf_s(valueBuffer, L"%u", item.m_faceIndex);
            DrawValue(valueBuffer);

            y += m_rowHeight;
        }
    }
}

int FontFaceListWindow::GetItemPixelHeight(float dpiScale, int itemIndex)
{
    return static_cast<int>(ceilf(m_fontItems[itemIndex].m_itemHeight * dpiScale));
}

