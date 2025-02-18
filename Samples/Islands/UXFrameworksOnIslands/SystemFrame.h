// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

#include "IFrame.h"
#include "IFrameHost.h"
#include "AutomationTree.h"
#include "EventRevokers.h"
#include "Output.h"

// Base class for frames using the system stack.
// This will pull together all the subsystems needed for a system frame
// and provide a way to connect child frames.
class SystemFrame : public IFrame, public SettingChangedHandler
{
public:
    SystemFrame(
        const winrt::DispatcherQueue& queue,
        const winrt::WUC::Compositor& systemCompositor,
        const std::shared_ptr<SettingCollection>& settings);

    [[nodiscard]] winrt::com_ptr<::IRawElementProviderFragmentRoot> GetAutomationProvider() const final override { return m_automationTree->Root(); }

    [[nodiscard]] winrt::ContentCoordinateConverter GetConverter() const final override { return m_island.CoordinateConverter(); }

    [[nodiscard]] winrt::ContentIsland GetIsland() const final override { return m_island; }

    [[nodiscard]] std::shared_ptr<VisualTreeNode> GetRootVisualTreeNode() const final override { return m_rootVisualTreeNode; }

    [[nodiscard]] virtual std::shared_ptr<FocusList> GetFocusList() const final override { return m_focusList; }

    [[nodiscard]] bool IsLiftedFrame() const final { return false; }

    LRESULT HandleMessage(
        UINT /*message*/,
        WPARAM /*wParam*/,
        LPARAM /*lParam*/,
        _Out_ bool* handled) override
    {
        // Default implementation does nothing.
        *handled = false;
        return 0;
    }

    void SetFrameHost(IFrameHost const* const frameHost) { m_frameHost = frameHost; m_automationTree->SetFrameHost(frameHost); }

    auto& GetOutput() noexcept { return m_output; }

protected:
    winrt::ChildSiteLink ConnectChildFrame(
        IFrame* frame,
        const winrt::WUC::ContainerVisual& childPlacementVisual);

    virtual void HandleContentLayout();

    IFrameHost const* m_frameHost = nullptr;
    std::shared_ptr<AutomationTree> m_automationTree = nullptr;
    std::shared_ptr<FocusList> m_focusList{};

private:
    void OnSettingChanged(SettingId id) override;

    SystemOutput m_output;
    winrt::ContentIsland m_island = nullptr;
    ContentIslandStateChanged_revoker m_islandStateChangedRevoker{};
    std::shared_ptr<VisualTreeNode> m_rootVisualTreeNode = nullptr;
};
