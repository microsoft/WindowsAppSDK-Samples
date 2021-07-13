// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

wil::com_ptr<IDWriteFontCollection3> CreateCustomFontCollection(DWRITE_FONT_FAMILY_MODEL fontFamilyModel);

wil::com_ptr<IDWriteFontSet3> CreateCustomFontSet();
