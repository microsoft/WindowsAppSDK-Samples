// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "TextBlock.h"

namespace winrt::DrawingIslandComponents::implementation
{
    TextBlock::TextBlock(winrt::com_ptr<TextRenderer> const& textRenderer) :
        m_textRenderer(textRenderer)
    {
    }
}
