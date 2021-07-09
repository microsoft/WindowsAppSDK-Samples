// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "MarkdownWindow.h"

class Scenario_BasicTextLayout final : public MarkdownWindow
{
public:
    Scenario_BasicTextLayout(HWND parentWindow, TextRenderer* textRenderer);
};
