// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "SplitWindow.h"
#include "FontFamilyListWindow.h"
#include "FontFaceListWindow.h"

class FontCollectionWindow : public SplitWindow
{
public:
    FontCollectionWindow(
        HWND parentWindow,
        TextRenderer* textRenderer,
        IDWriteFontCollection3* fontCollection = nullptr,
        DWRITE_FONT_FAMILY_MODEL fontFamilyModel = DWRITE_FONT_FAMILY_MODEL_TYPOGRAPHIC
        );

protected:
    LRESULT OnControlCommand(HWND controlWindow, uint32_t controlId, uint32_t notificationCode) override;

private:
    FontFamilyListWindow& GetFamilyList();
    FontFaceListWindow& GetFaceList();

    void InitializeFaceList();
};
