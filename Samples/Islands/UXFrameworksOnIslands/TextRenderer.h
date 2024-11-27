// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

extern winrt::com_ptr<IDWriteFactory7> const g_dwriteFactory;

// Creates an object that encapsulates text formatting properties.
winrt::com_ptr<IDWriteTextFormat> CreateTextFormat(
    _In_z_ WCHAR const* familyName,
    float fontSize,
    DWRITE_FONT_WEIGHT fontWeight = DWRITE_FONT_WEIGHT_NORMAL,
    DWRITE_FONT_STYLE fontStyle = DWRITE_FONT_STYLE_NORMAL,
    DWRITE_FONT_STRETCH fontStretch = DWRITE_FONT_STRETCH_NORMAL);

// Creates an object that represents a formatted text string.
winrt::com_ptr<IDWriteTextLayout> CreateTextLayout(_In_z_ WCHAR const* text, _In_opt_ IDWriteTextFormat* textFormat = nullptr);

// Computes the logical width and height of the text layout object.
// Note: These are layout bounds, not ink bounds.
winrt::Size MeasureTextLayout(IDWriteTextLayout* textLayout);
