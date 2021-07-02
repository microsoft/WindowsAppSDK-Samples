// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "Main.h"
#include "Helpers.h"
#include "Resource.h"
#include "Scenario_BasicTextLayout.h"
#include "Scenarios.h"

std::unique_ptr<ChildWindow> CreateScenario_BasicTextLayout(HWND parentWindow, TextRenderer* textRenderer)
{
    return std::make_unique<Scenario_BasicTextLayout>(parentWindow, textRenderer);
}

Scenario_BasicTextLayout::Scenario_BasicTextLayout(HWND parentWindow, TextRenderer* textRenderer) :
    MarkdownWindow{ parentWindow, textRenderer, ID_INTRODUCTION_MARKDOWN }
{
}
