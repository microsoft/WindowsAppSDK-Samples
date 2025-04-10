// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

#include <UIAutomation.h>
#include "VisualTreeNode.h"
#include "FocusList.h"

struct IFrame
{
    virtual ~IFrame() = default;

    [[nodiscard]] virtual winrt::com_ptr<::IRawElementProviderFragmentRoot> GetAutomationProvider() const = 0;

    [[nodiscard]] virtual winrt::ContentCoordinateConverter GetConverter() const = 0;

    [[nodiscard]] virtual winrt::ContentIsland GetIsland() const = 0;

    [[nodiscard]] virtual std::shared_ptr<VisualTreeNode> GetRootVisualTreeNode() const = 0;

    [[nodiscard]] virtual std::shared_ptr<FocusList> GetFocusList() const = 0;

    [[nodiscard]] virtual bool IsLiftedFrame() const = 0;

    virtual bool SystemPreTranslateMessage(
        UINT message,
        WPARAM wParam,
        LPARAM lParam) = 0;

    virtual LRESULT HandleMessage(
        UINT message,
        WPARAM wParam,
        LPARAM lParam,
        _Out_ bool* handled) = 0;
};
