// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "ListWindow.h"

class FontFaceListWindow final : public ListWindow
{
public:
    FontFaceListWindow(HWND parentWindow, TextRenderer* textRenderer);
    
    void SetFontFamily(
        _In_ IDWriteFontFamily2* fontFamily,
        DWRITE_FONT_FAMILY_MODEL fontFamilyModel,
        _In_opt_ IDWriteFontCollection3* customFontCollection
        );

protected:
    void DrawItem(TextRenderer* textRenderer, int itemIndex, bool isSelected) override;
    int GetItemPixelHeight(float dpiScale, int itemIndex) override;

private:
    struct AxisInfo
    {
        DWRITE_FONT_AXIS_TAG axisTag;
        float value;
        float minValue;
        float maxValue;
    };

    struct FontItem
    {
        wil::com_ptr<IDWriteTextLayout4> m_faceNameTextLayout;

        std::wstring m_fileName;
        int m_faceIndex = -1;

        std::wstring m_typoFamilyName;
        std::wstring m_wssFamilyName;
        std::wstring m_gdiFamilyName;
        std::wstring m_fullName;
        std::wstring m_postName;

        std::vector<AxisInfo> m_axes;

        float m_faceNameHeight = 0;
        float m_itemHeight = 0;
    };

    wil::com_ptr<IDWriteTextLayout4> CreateLabel(uint32_t stringId);

    wil::com_ptr<IDWriteTextFormat3> m_labelTextFormat;
    wil::com_ptr<IDWriteTextFormat3> m_valueTextFormat;

    wil::com_ptr<IDWriteTextLayout4> m_fileNameLabel;
    wil::com_ptr<IDWriteTextLayout4> m_faceIndexLabel;
    wil::com_ptr<IDWriteTextLayout4> m_typoFamilyNameLabel;
    wil::com_ptr<IDWriteTextLayout4> m_wssFamilyNameLabel;
    wil::com_ptr<IDWriteTextLayout4> m_gdiFamilyNameLabel;
    wil::com_ptr<IDWriteTextLayout4> m_fullNameLabel;
    wil::com_ptr<IDWriteTextLayout4> m_postNameLabel;

    float m_maxLabelWidth = 0;
    float m_rowHeight = 0;

    std::vector<FontItem> m_fontItems;
};
