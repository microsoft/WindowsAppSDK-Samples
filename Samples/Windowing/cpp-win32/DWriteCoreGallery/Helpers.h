// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

/// <summary>
/// Creates a text format object using the typographic font family model.
/// </summary>
wil::com_ptr<IDWriteTextFormat3> CreateTextFormat(
    _In_z_ wchar_t const* typographicFamilyName,
    float fontSize,
    std::span<DWRITE_FONT_AXIS_VALUE const> axisValues,
    IDWriteFontCollection* fontCollection = nullptr
);

wil::com_ptr<IDWriteTextLayout4> CreateTextLayout(IDWriteTextFormat3* textFormat, std::span<wchar_t const> text);
wil::com_ptr<IDWriteTextLayout4> CreateTextLayout(IDWriteTextFormat3* textFormat, _In_z_ wchar_t const* text);

std::wstring GetFontFilePath(IDWriteFontFace* fontFace);
std::wstring GetLocalString(IDWriteLocalizedStrings* localizedStrings);
std::wstring GetPropertyString(IDWriteFontSet2* fontSet, uint32_t fontIndex, DWRITE_FONT_PROPERTY_ID propertyId);
std::wstring GetInformationalString(IDWriteFontFace6* fontFace, DWRITE_INFORMATIONAL_STRING_ID stringId);
std::wstring GetFamilyName(IDWriteFontFace6* fontFace, DWRITE_FONT_FAMILY_MODEL fontFamilyModel);
std::wstring GetFaceName(IDWriteFontFace6* fontFace, DWRITE_FONT_FAMILY_MODEL fontFamilyModel);

inline std::wstring GetTypoFamilyName(IDWriteFontFace6* fontFace)
{
    return GetFamilyName(fontFace, DWRITE_FONT_FAMILY_MODEL_TYPOGRAPHIC);
}

inline std::wstring GetTypoFaceName(IDWriteFontFace6* fontFace)
{
    return GetFaceName(fontFace, DWRITE_FONT_FAMILY_MODEL_TYPOGRAPHIC);
}

std::vector<DWRITE_FONT_AXIS_VALUE> GetFontAxisValues(IDWriteFontFace6* fontFace);
std::vector<DWRITE_FONT_AXIS_RANGE> GetFontAxisRanges(IDWriteFontFace6* fontFace);
