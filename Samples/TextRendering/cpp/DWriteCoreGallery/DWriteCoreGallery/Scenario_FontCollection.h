// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

class Scenario_FontCollection final : public SplitWindow
{
public:
    Scenario_FontCollection(HWND parentWindow, TextRenderer* textRenderer, IDWriteFontCollection3* fontCollection, DWRITE_FONT_FAMILY_MODEL fontFamilyModel, uint32_t resourceId);
};
