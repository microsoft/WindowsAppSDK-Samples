// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

#include "ChildWindow.h"

//
// Factory functions for each type of scenario.
// These are defined in their respective source files.
//
std::unique_ptr<ChildWindow> CreateScenario_BasicTextLayout(HWND parentWindow, TextRenderer* textRenderer);
std::unique_ptr<ChildWindow> CreateScenario_OpticalSize(HWND parentWindow, TextRenderer* textRenderer);
std::unique_ptr<ChildWindow> CreateScenario_SystemFontCollectionTypographic(HWND parentWindow, TextRenderer* textRenderer);
std::unique_ptr<ChildWindow> CreateScenario_SystemFontCollectionWeightStretchStyle(HWND parentWindow, TextRenderer* textRenderer);
std::unique_ptr<ChildWindow> CreateScenario_CustomFontCollection(HWND parentWindow, TextRenderer* textRenderer);
