// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "Main.h"
#include "Helpers.h"
#include "Resource.h"

wil::com_ptr<IDWriteTextFormat3> CreateTextFormat(
    _In_z_ wchar_t const* typographicFamilyName,
    float fontSize,
    std::span<DWRITE_FONT_AXIS_VALUE const> axisValues,
    IDWriteFontCollection* fontCollection
    )
{
    // Use the current user's locale name as the language of the text.
    // This affects font fallback. For example, the same character might fall back to different
    // fonts for Chinese, Japanese, or Korean users.
    wchar_t localeName[LOCALE_NAME_MAX_LENGTH];
    THROW_LAST_ERROR_IF(0 == GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH));
    
    // Create the text format object.
    // This overload of CreateTextFormat uses the typographic font family model, which means
    //  1. The family name is a typographic family name (e.g., "Sitka") instead of a weight-stretch-style family name (e.g., "Sitka Text"), and
    //  2. The font style is specified as an open-ended array of axis values instead of weigth, stretch, and style enum values.
    wil::com_ptr<IDWriteTextFormat3> textFormat;
    THROW_IF_FAILED(g_factory->CreateTextFormat(typographicFamilyName, fontCollection, axisValues.data(), static_cast<uint32_t>(axisValues.size()), fontSize, localeName, &textFormat));

    // Tell the text layout engine to automatically set the value of the 'opsz' axis based on the font size,
    // unless an 'opsz' axis value is explicitly specified.
    THROW_IF_FAILED(textFormat->SetAutomaticFontAxes(DWRITE_AUTOMATIC_FONT_AXES_OPTICAL_SIZE));

    return textFormat;
}

wil::com_ptr<IDWriteTextLayout4> CreateTextLayout(IDWriteTextFormat3* textFormat, std::span<wchar_t const> text)
{
    wil::com_ptr<IDWriteTextLayout> textLayout;
    g_factory->CreateTextLayout(text.data(), static_cast<uint32_t>(text.size()), textFormat, /*maxWidth*/ 0.0f, /*maxHeight*/ 0.0f, &textLayout);
    return textLayout.query<IDWriteTextLayout4>();
}

wil::com_ptr<IDWriteTextLayout4> CreateTextLayout(IDWriteTextFormat3* textFormat, _In_z_ wchar_t const* text)
{
    return CreateTextLayout(textFormat, std::span<wchar_t const>(text, wcslen(text)));
}

std::wstring GetFontFilePath(IDWriteFontFace* fontFace)
{
    wil::com_ptr<IDWriteFontFile> fileReference;
    uint32_t fileCount = 1;
    THROW_IF_FAILED(fontFace->GetFiles(&fileCount, &fileReference));

    wil::com_ptr<IDWriteFontFileLoader> fileLoader;
    THROW_IF_FAILED(fileReference->GetLoader(&fileLoader));

    auto localLoader = fileLoader.try_query<IDWriteLocalFontFileLoader>();
    if (localLoader == nullptr)
    {
        return {};
    }

    void const* fileKey;
    uint32_t fileKeySize;
    THROW_IF_FAILED(fileReference->GetReferenceKey(&fileKey, &fileKeySize));

    WCHAR path[MAX_PATH];
    THROW_IF_FAILED(localLoader->GetFilePathFromKey(fileKey, fileKeySize, path, MAX_PATH));

    return path;
}

std::wstring GetLocalString(IDWriteLocalizedStrings* localizedStrings)
{
    std::wstring result;

    uint32_t index;
    BOOL exists;
    THROW_IF_FAILED(localizedStrings->FindLocaleName(L"en-us", &index, &exists));

    if (!exists)
    {
        if (localizedStrings->GetCount() == 0)
        {
            return result;
        }

        index = 0;
    }

    uint32_t length;
    THROW_IF_FAILED(localizedStrings->GetStringLength(index, &length));

    if (length != 0)
    {
        result.resize(length);
        THROW_IF_FAILED(localizedStrings->GetString(index, &result[0], length + 1u));
    }

    return result;
}

std::wstring GetPropertyString(IDWriteFontSet2* fontSet, uint32_t fontIndex, DWRITE_FONT_PROPERTY_ID propertyId)
{
    wil::com_ptr<IDWriteLocalizedStrings> localizedStrings;
    BOOL exists;
    THROW_IF_FAILED(fontSet->GetPropertyValues(fontIndex, propertyId, &exists, &localizedStrings));

    if (!exists)
    {
        return {};
    }

    return GetLocalString(localizedStrings.get());
}

std::wstring GetInformationalString(IDWriteFontFace6* fontFace, DWRITE_INFORMATIONAL_STRING_ID stringId)
{
    wil::com_ptr<IDWriteLocalizedStrings> localizedStrings;
    BOOL exists;
    THROW_IF_FAILED(fontFace->GetInformationalStrings(stringId, &localizedStrings, &exists));
    return exists ? GetLocalString(localizedStrings.get()) : std::wstring{};
}

std::wstring GetFamilyName(IDWriteFontFace6* fontFace, DWRITE_FONT_FAMILY_MODEL fontFamilyModel)
{
    wil::com_ptr<IDWriteLocalizedStrings> localizedStrings;
    THROW_IF_FAILED(fontFace->GetFamilyNames(DWRITE_FONT_FAMILY_MODEL_TYPOGRAPHIC, &localizedStrings));
    return GetLocalString(localizedStrings.get());
}

std::wstring GetFaceName(IDWriteFontFace6* fontFace, DWRITE_FONT_FAMILY_MODEL fontFamilyModel)
{
    wil::com_ptr<IDWriteLocalizedStrings> localizedStrings;
    THROW_IF_FAILED(fontFace->GetFaceNames(DWRITE_FONT_FAMILY_MODEL_TYPOGRAPHIC, &localizedStrings));
    return GetLocalString(localizedStrings.get());
}

std::vector<DWRITE_FONT_AXIS_VALUE> GetFontAxisValues(IDWriteFontFace6* fontFace)
{
    uint32_t axisCount = fontFace->GetFontAxisValueCount();

    std::vector<DWRITE_FONT_AXIS_VALUE> axisValues(axisCount);
    THROW_IF_FAILED(fontFace->GetFontAxisValues(axisValues.data(), axisCount));

    return axisValues;
}

std::vector<DWRITE_FONT_AXIS_RANGE> GetFontAxisRanges(IDWriteFontFace6* fontFace)
{
    wil::com_ptr<IDWriteFontResource> fontResource;
    THROW_IF_FAILED(fontFace->GetFontResource(&fontResource));

    uint32_t axisCount = fontResource->GetFontAxisCount();

    std::vector<DWRITE_FONT_AXIS_RANGE> axisRanges(axisCount);
    THROW_IF_FAILED(fontResource->GetFontAxisRanges(axisRanges.data(), axisCount));

    return axisRanges;

}
