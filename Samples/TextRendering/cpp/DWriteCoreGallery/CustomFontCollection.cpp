// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "Main.h"
#include "Helpers.h"
#include "Resource.h"
#include "ResourceFontFileLoader.h"
#include "CustomFontCollection.h"

wil::com_ptr<IDWriteFontCollection3> CreateCustomFontCollection(DWRITE_FONT_FAMILY_MODEL fontFamilyModel)
{
    // The easiest way to create a custom font collection is to first create a custom font set.
    auto fontSet = CreateCustomFontSet();

    // A font collection is essentially a hierarchical view of a font set, where the fonts are
    // grouped into families according to the specified font family model.
    wil::com_ptr<IDWriteFontCollection2> fontCollection;
    THROW_IF_FAILED(g_factory->CreateFontCollectionFromFontSet(fontSet.get(), fontFamilyModel, &fontCollection));

    return fontCollection.query<IDWriteFontCollection3>();
}

wil::com_ptr<IDWriteFontSet3> CreateCustomFontSet()
{
    // We'll use a font set builder to create a custom font set. The pattern is:
    //  1. Create the builder object
    //  2. Add font files to the builder
    //  3. Call CreateFontSet on the builder
    wil::com_ptr<IDWriteFontSetBuilder2> fontSetBuilder;
    THROW_IF_FAILED(g_factory->CreateFontSetBuilder(&fontSetBuilder));

    // Add each font file. In this example, the font files are binary resources.
    for (uint32_t resourceId = FIRST_FONT_RESOURCE_ID; resourceId <= LAST_FONT_RESOURCE_ID; resourceId++)
    {
        // In this example the font files are binary resources, so the application implements a custom
        // font file loader to provide access to those files. Call a helper method implemented by the
        // loader to create a font file reference.
        wil::com_ptr<IDWriteFontFile> fontFile = ResourceFontFileLoader::CreateFontFileReference(resourceId);

        // Pass the font file reference to the builder's AddFontFile method.
        // Note: There is another overload of this method that takes a file path as a strong, so
        // one can easily create a custom font set of ordinary font files without implementing any
        // callback interfaces.
        THROW_IF_FAILED(fontSetBuilder->AddFontFile(fontFile.get()));
    }

    // Create the font set.
    wil::com_ptr<IDWriteFontSet> fontSet;
    THROW_IF_FAILED(fontSetBuilder->CreateFontSet(&fontSet));

    return fontSet.query<IDWriteFontSet3>();
}
