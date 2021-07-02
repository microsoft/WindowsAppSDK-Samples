// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "ListWindow.h"

class FontFamilyListWindow final : public ListWindow
{
public:
    FontFamilyListWindow(
        HWND parentWindow,
        TextRenderer* textRenderer,
        IDWriteFontCollection3* fontCollection = nullptr,
        DWRITE_FONT_FAMILY_MODEL fontFamilyModel = DWRITE_FONT_FAMILY_MODEL_TYPOGRAPHIC
    );

    _Ret_maybenull_ IDWriteFontCollection3* GetInputFontCollection() const noexcept
    {
        return m_inputFontCollection.get();
    }

    IDWriteFontCollection3* GetFontCollection() const noexcept
    {
        return m_fontCollection.get();
    }

    std::wstring const& GetFamilyName(size_t index) const noexcept
    {
        return m_familyNames[index];
    }

    DWRITE_FONT_FAMILY_MODEL GetFontFamilyModel() const noexcept
    {
        return m_fontFamilyModel;
    }

protected:
    void DrawItem(TextRenderer* textRenderer, int itemIndex, bool isSelected) override;
    int GetItemPixelHeight(float dpiScale, int itemIndex) override;

private:
    wil::com_ptr<IDWriteFontCollection3> m_inputFontCollection;
    wil::com_ptr<IDWriteFontCollection3> m_fontCollection;
    wil::com_ptr<IDWriteTextFormat3> m_textFormat;
    DWRITE_FONT_FAMILY_MODEL m_fontFamilyModel;
    std::vector<std::wstring> m_familyNames;
    float m_itemHeight = 0;
};
