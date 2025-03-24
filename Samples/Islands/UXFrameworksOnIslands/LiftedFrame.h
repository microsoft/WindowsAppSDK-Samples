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
class LiftedFrame : public IFrame, public SettingChangedHandler
{
public:
    LiftedFrame(const winrt::Compositor& compositor, const std::shared_ptr<SettingCollection>& settings);

    [[nodiscard]] winrt::com_ptr<::IRawElementProviderFragmentRoot> GetAutomationProvider() const final override { return m_automationTree->Root(); }

    [[nodiscard]] winrt::ContentCoordinateConverter GetConverter() const final override { return m_island.CoordinateConverter(); }

    [[nodiscard]] winrt::ContentIsland GetIsland() const final override { return m_island; }

    [[nodiscard]] std::shared_ptr<VisualTreeNode> GetRootVisualTreeNode() const final override { return m_rootVisualTreeNode; }

    [[nodiscard]] virtual std::shared_ptr<FocusList> GetFocusList() const final override { return m_focusList; }

    [[nodiscard]] bool IsLiftedFrame() const final { return true; }

    bool SystemPreTranslateMessage(
        UINT /*message*/,
        WPARAM /*wParam*/,
        LPARAM /*lParam*/) override
    {
        // We don't expect to receive pretranslate messages in a lifted frame.
        return false;
    }

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

    virtual void OnPreTranslateDirectMessage(
        const MSG* msg,
        UINT keyboardModifiers,
        _Inout_ bool* handled);

    virtual void OnPreTranslateTreeMessage(
        const MSG* msg,
        UINT keyboardModifiers,
        _Inout_ bool* handled);

protected:
    winrt::ChildSiteLink ConnectChildFrame(
        IFrame* frame,
        const winrt::ContainerVisual& childPlacementVisual);

    winrt::DesktopPopupSiteBridge ConnectPopupFrame(
        IFrame* frame);

    virtual void HandleContentLayout();

    IFrameHost const* m_frameHost = nullptr;
    std::shared_ptr<AutomationTree> m_automationTree = nullptr;
    std::shared_ptr<FocusList> m_focusList{};

private:
    void OnSettingChanged(SettingId id) override;

    LiftedOutput m_output;
    winrt::ContentIsland m_island = nullptr;
    ContentIslandStateChanged_revoker m_islandStateChangedRevoker{};
    std::shared_ptr<VisualTreeNode> m_rootVisualTreeNode = nullptr;
};
