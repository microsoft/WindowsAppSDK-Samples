// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "Main.h"
#include "Helpers.h"
#include "Resource.h"
#include "MarkdownWindow.h"
#include "FontCollectionWindow.h"
#include "CustomFontCollection.h"
#include "Scenario_FontCollection.h"
#include "Scenarios.h"

std::unique_ptr<ChildWindow> CreateScenario_SystemFontCollectionTypographic(HWND parentWindow, TextRenderer* textRenderer)
{
    return std::make_unique<Scenario_FontCollection>(
        parentWindow, 
        textRenderer, 
        nullptr, 
        DWRITE_FONT_FAMILY_MODEL_TYPOGRAPHIC, 
        ID_SYSTEM_FONT_COLLECTION_TYPOGRAPHIC_MARKDOWN
        );
}

std::unique_ptr<ChildWindow> CreateScenario_SystemFontCollectionWeightStretchStyle(HWND parentWindow, TextRenderer* textRenderer)
{
    return std::make_unique<Scenario_FontCollection>(
        parentWindow, 
        textRenderer, 
        nullptr, 
        DWRITE_FONT_FAMILY_MODEL_WEIGHT_STRETCH_STYLE, 
        ID_SYSTEM_FONT_COLLECTION_WSS_MARKDOWN
        );
}

std::unique_ptr<ChildWindow> CreateScenario_CustomFontCollection(HWND parentWindow, TextRenderer* textRenderer)
{
    return std::make_unique<Scenario_FontCollection>(
        parentWindow,
        textRenderer,
        CreateCustomFontCollection(DWRITE_FONT_FAMILY_MODEL_TYPOGRAPHIC).get(),
        DWRITE_FONT_FAMILY_MODEL_TYPOGRAPHIC,
        ID_CUSTOM_FONT_COLLECTION_MARKDOWN
        );
}

Scenario_FontCollection::Scenario_FontCollection(HWND parentWindow, TextRenderer* textRenderer, IDWriteFontCollection3* fontCollection, DWRITE_FONT_FAMILY_MODEL fontFamilyModel, uint32_t resourceId) :
    SplitWindow{ parentWindow, textRenderer, 500.0f }
{
    Initialize(
        std::make_unique<MarkdownWindow>(GetHandle(), textRenderer, resourceId),
        std::make_unique<FontCollectionWindow>(GetHandle(), textRenderer, fontCollection, fontFamilyModel)
    );
}
