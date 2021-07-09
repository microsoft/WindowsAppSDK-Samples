// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "TextRenderer.h"
#include "ChildWindow.h"

class MainWindow final
{
public:
    explicit MainWindow(int nShowCmd);

private:

    // Window message handlers.
    LRESULT OnCreate(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT OnSize(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT OnFocus(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT OnCommand(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT OnDpiChange(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept;

    ChildWindow* GetScenarioWindow(uint32_t menuItemId);
    void ResizeScenarioWindow();
    void SetScenario(uint32_t menuItemId);

    // The following members are initialized by OnCreate.
    HWND m_hwnd = nullptr;
    wil::com_ptr<TextRenderer> m_textRenderer;
    std::map<uint32_t, std::unique_ptr<ChildWindow>> m_scenariosById;
    ChildWindow* m_activeScenarioWindow = nullptr;
    uint32_t m_activeScenarioId = 0;
};
