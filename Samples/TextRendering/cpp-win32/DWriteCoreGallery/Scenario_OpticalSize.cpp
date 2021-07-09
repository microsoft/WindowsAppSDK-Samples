// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "Main.h"
#include "Helpers.h"
#include "Resource.h"
#include "SplitWindow.h"
#include "MarkdownWindow.h"

std::unique_ptr<ChildWindow> CreateScenario_OpticalSize(HWND parentWindow, TextRenderer* textRenderer)
{
    static const uint32_t fontSizes[] = { 6, 11, 17, 22, 25, 28 };
    std::vector<DWRITE_TEXT_RANGE> textRanges;

    std::wstring bodyText;

    for (uint32_t fontSize : fontSizes)
    {
        wchar_t lineText[20];
        uint32_t lineLength = swprintf_s(lineText, L"Sitka %upt\n", fontSize);

        textRanges.push_back(DWRITE_TEXT_RANGE{ static_cast<uint32_t>(bodyText.length()), lineLength });
        bodyText.append(lineText, lineLength);
    }

    auto headingFormat = CreateTextFormat(g_defaultStyle.headingFamilyName, g_defaultStyle.headingFontSize, g_defaultStyle.headingAxisValues);
    auto bodyFormat = CreateTextFormat(L"Sitka", g_defaultStyle.bodyFontSize, g_defaultStyle.bodyAxisValues);

    auto MakeTextWindow = [&](HWND parent, wchar_t const* headingText, float opszValue)
    {
        auto headingTextLayout = CreateTextLayout(headingFormat.get(), headingText);
        auto bodyTextLayout = CreateTextLayout(bodyFormat.get(), bodyText);

        // Set the font size of each line.
        for (uint32_t i = 0; i < std::size(fontSizes); i++)
        {
            constexpr float pointsToDips = 96.0f / 72.0f;
            THROW_IF_FAILED(bodyTextLayout->SetFontSize(fontSizes[i] * pointsToDips, textRanges[i]));
        }

        if (opszValue == 0)
        {
            // Let DWrite automatically set the opsz axis value based on the font size.
            THROW_IF_FAILED(bodyTextLayout->SetAutomaticFontAxes(DWRITE_AUTOMATIC_FONT_AXES_OPTICAL_SIZE));
        }
        else
        {
            // Explicitly set the opsz axis value.
            DWRITE_FONT_AXIS_VALUE axisValues[] =
            {
                { DWRITE_FONT_AXIS_TAG_WEIGHT, 400 },
                { DWRITE_FONT_AXIS_TAG_ITALIC, 0 },
                { DWRITE_FONT_AXIS_TAG_OPTICAL_SIZE, opszValue }
            };
            THROW_IF_FAILED(bodyTextLayout->SetFontAxisValues(axisValues, ARRAYSIZE(axisValues), DWRITE_TEXT_RANGE{ 0, UINT32_MAX }));
        }

        return std::make_unique<StaticTextWindow>(parent, textRenderer, std::vector<wil::com_ptr<IDWriteTextLayout4>>{ headingTextLayout, bodyTextLayout });
    };

    // Create a four-column layout using a SplitWindow hierarchy, as follows:
    //
    //   split0 +-------------> column0 (MarkdownWindow)
    //          |
    //          V
    //        split1 +--------> column1 (StaticTextWindow with opsz = Automatic)
    //               |
    //               V
    //             split2 +---> column2 (StaticTextWindow with opsz = 6)
    //                    |
    //                    |
    //                    +---> column3 (StaticTextWindow with opszs = 28)
    //                          
    auto split0 = std::make_unique<SplitWindow>(parentWindow, textRenderer, /*leftWidth*/ 400.0f);
    auto split1 = std::make_unique<SplitWindow>(split0->GetHandle(), textRenderer, /*leftWidth*/ 250.0f);
    auto split2 = std::make_unique<SplitWindow>(split1->GetHandle(), textRenderer, /*leftWidth*/ 250.0f);

    auto column0 = std::make_unique<MarkdownWindow>(split0->GetHandle(), textRenderer, ID_OPTICAL_SIZE_MARKDOWN);
    auto column1 = MakeTextWindow(split1->GetHandle(), L"opsz = Auto", 0);
    auto column2 = MakeTextWindow(split2->GetHandle(), L"opsz = 6", 6);
    auto column3 = MakeTextWindow(split2->GetHandle(), L"opsz = 28", 28);

    split2->Initialize(std::move(column2), std::move(column3));
    split1->Initialize(std::move(column1), std::move(split2));
    split0->Initialize(std::move(column0), std::move(split1));

    return split0;
}
