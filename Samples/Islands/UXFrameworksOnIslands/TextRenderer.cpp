// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"
#include "TextRenderer.h"
#include "CompositionDeviceResource.h"

namespace
{
    winrt::com_ptr<IDWriteFactory7> CreateDWriteFactory()
    {
        winrt::com_ptr<IDWriteFactory7> factory;

        // Create the DWrite factory object.
        winrt::check_hresult(DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(decltype(*factory)),
            reinterpret_cast<IUnknown**>(factory.put())));

        return factory;
    }
}

winrt::com_ptr<IDWriteFactory7> const g_dwriteFactory = CreateDWriteFactory();

// Creates an object that encapsulates text formatting properties.
winrt::com_ptr<IDWriteTextFormat> CreateTextFormat(
    _In_z_ WCHAR const* familyName,
    float fontSize,
    DWRITE_FONT_WEIGHT fontWeight,
    DWRITE_FONT_STYLE fontStyle,
    DWRITE_FONT_STRETCH fontStretch)
{
    winrt::com_ptr<IDWriteTextFormat> textFormat;

    winrt::check_hresult(g_dwriteFactory->CreateTextFormat(
        familyName,
        nullptr,
        fontWeight,
        fontStyle,
        fontStretch,
        fontSize,
        L"en-US",
        textFormat.put()));

    winrt::check_hresult(textFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP));

    return textFormat;
}

winrt::com_ptr<IDWriteTextFormat> const g_defaultTextFormat = CreateTextFormat(L"Segoe UI", /*fontSize*/ 10.0f);

// Creates an object that represents a formatted text string.
winrt::com_ptr<IDWriteTextLayout> CreateTextLayout(_In_z_ WCHAR const* text, _In_opt_ IDWriteTextFormat* textFormat)
{
    winrt::com_ptr<IDWriteTextLayout> textLayout;
    winrt::check_hresult(g_dwriteFactory->CreateTextLayout(
        text,
        static_cast<uint32_t>(wcslen(text)),
        textFormat != nullptr ? textFormat : g_defaultTextFormat.get(),
        /*maxWidth*/ 0,
        /*maxHeight*/ 0,
        /*out*/ textLayout.put()));
    return textLayout;
}

// Computes the logical width and height of the text layout object.
// Note: These are layout bounds, not ink bounds.
winrt::Size MeasureTextLayout(IDWriteTextLayout* textLayout)
{
    // Get the metrics from the text layout.
    DWRITE_TEXT_METRICS textMetrics;
    winrt::check_hresult(textLayout->GetMetrics(/*out*/ &textMetrics));

    return { textMetrics.width, textMetrics.height };
}
