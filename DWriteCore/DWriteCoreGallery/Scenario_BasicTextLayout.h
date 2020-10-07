#pragma once
#include "MarkdownWindow.h"

class Scenario_BasicTextLayout final : public MarkdownWindow
{
public:
    Scenario_BasicTextLayout(HWND parentWindow, TextRenderer* textRenderer);
};
