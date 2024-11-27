// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

#include "IFrame.h"
#include "IFrameHost.h"
#include "AutomationTree.h"
#include "EventRevokers.h"
#include "Output.h"

// Base class for frames using the lifted stack.
// This will pull together all the subsystems needed for a lifted frame
// and provide a way to connect child frames.
class LiftedFrame : public IFrame
{
public:
    explicit LiftedFrame(const winrt::Compositor& compositor);

    [[nodiscard]] winrt::com_ptr<::IRawElementProviderFragmentRoot> GetAutomationProvider() const final override { return m_automationTree->Root(); }

    [[nodiscard]] winrt::ContentCoordinateConverter GetConverter() const final override { return m_island.CoordinateConverter(); }

    [[nodiscard]] winrt::ContentIsland GetIsland() const final override { return m_island; }

    [[nodiscard]] std::shared_ptr<VisualTreeNode> GetRootVisualTreeNode() const final override { return m_rootVisualTreeNode; }

    [[nodiscard]] bool IsLiftedFrame() const final { return true; }

    LRESULT HandleMessage(
        UINT /*message*/,
        WPARAM /*wParam*/,
        LPARAM /*lParam*/,
        _Out_ bool* handled) final
    {
        // We don't expect to receive messages in a lifted frame.
        *handled = false;
        return false;
    }

    auto& GetOutput() noexcept { return m_output; }

protected:
    winrt::ChildContentLink ConnectChildFrame(
        IFrame* frame,
        const winrt::ContainerVisual& childPlacementVisual);

    virtual void HandleContentLayout();

    IFrameHost const* m_frameHost = nullptr;
    std::shared_ptr<AutomationTree> m_automationTree = nullptr;

private:
    LiftedOutput m_output;
    winrt::ContentIsland m_island = nullptr;
    ContentIslandStateChanged_revoker m_islandStateChangedRevoker{};
    std::shared_ptr<VisualTreeNode> m_rootVisualTreeNode = nullptr;
};
