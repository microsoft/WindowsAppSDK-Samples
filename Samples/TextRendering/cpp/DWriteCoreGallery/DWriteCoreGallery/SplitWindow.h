// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "ChildWindow.h"

class SplitWindow : public ChildWindow
{
public:
    SplitWindow(HWND parentWindow, TextRenderer* textRenderer, float leftWidth = 300);

    void Initialize(std::unique_ptr<ChildWindow>&& leftPane, std::unique_ptr<ChildWindow>&& rightPane);

protected:
    void OnSize() override;

    ChildWindow* GetLeftPane() const noexcept
    {
        return m_leftPane.get();
    }

    ChildWindow* GetRightPane() const noexcept
    {
        return m_rightPane.get();
    }

private:
    std::unique_ptr<ChildWindow> m_leftPane;
    std::unique_ptr<ChildWindow> m_rightPane;
    float m_leftWidth;
};
