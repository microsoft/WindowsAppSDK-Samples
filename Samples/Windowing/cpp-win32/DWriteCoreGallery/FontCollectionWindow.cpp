// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "Main.h"
#include "Helpers.h"
#include "FontCollectionWindow.h"

FontCollectionWindow::FontCollectionWindow(
    HWND parentWindow,
    TextRenderer* textRenderer,
    IDWriteFontCollection3* fontCollection,
    DWRITE_FONT_FAMILY_MODEL fontFamilyModel
) : SplitWindow(parentWindow, textRenderer, /*leftWidth*/ 250)
{
    SplitWindow::Initialize(
        std::make_unique<FontFamilyListWindow>(GetHandle(), textRenderer, fontCollection, fontFamilyModel),
        std::make_unique<FontFaceListWindow>(GetHandle(), textRenderer)
    );

    GetFamilyList().SetSelectedIndex(0);
}

FontFamilyListWindow& FontCollectionWindow::GetFamilyList()
{
    return static_cast<FontFamilyListWindow&>(*GetLeftPane());
}

FontFaceListWindow& FontCollectionWindow::GetFaceList()
{
    return static_cast<FontFaceListWindow&>(*GetRightPane());
}

LRESULT FontCollectionWindow::OnControlCommand(HWND controlWindow, uint32_t controlId, uint32_t notificationCode)
{
    switch (notificationCode)
    {
    case LBN_SELCHANGE:
        InitializeFaceList();
        break;
    }
    return 0;
}

void FontCollectionWindow::InitializeFaceList()
{
    auto& familyList = GetFamilyList();

    int listIndex = familyList.GetSelectedIndex();
    if (listIndex >= 0)
    {
        std::wstring const& familyName = familyList.GetFamilyName(listIndex);
        IDWriteFontCollection3* fontCollection = familyList.GetFontCollection();

        uint32_t familyIndex;
        BOOL familyExists;
        THROW_IF_FAILED(fontCollection->FindFamilyName(familyName.c_str(), &familyIndex, &familyExists));
        
        if (familyExists)
        {
            wil::com_ptr<IDWriteFontFamily2> fontFamily;
            THROW_IF_FAILED(fontCollection->GetFontFamily(familyIndex, &fontFamily));

            GetFaceList().SetFontFamily(fontFamily.get(), fontCollection->GetFontFamilyModel(), familyList.GetInputFontCollection());
        }
    }
}

