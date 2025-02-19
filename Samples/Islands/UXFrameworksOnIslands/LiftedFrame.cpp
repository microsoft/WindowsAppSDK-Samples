// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"
#include "LiftedFrame.h"

LiftedFrame::LiftedFrame(const winrt::Compositor& compositor, const std::shared_ptr<SettingCollection>& settings) : 
    SettingChangedHandler(settings),
    m_output(compositor, settings)
{
    auto rootVisual = compositor.CreateContainerVisual();
    rootVisual.RelativeSizeAdjustment({1.0f, 1.0f});
    m_rootVisualTreeNode = VisualTreeNode::Create(rootVisual.as<::IUnknown>());
    m_island = winrt::ContentIsland::Create(rootVisual);
    m_automationTree = AutomationTree::Create(this, L"LiftedFrame", UIA_PaneControlTypeId);

    m_focusList = FocusList::Create();
    m_focusList->AddVisual(m_rootVisualTreeNode);

    // Set up a handler for size changes
    m_islandStateChangedRevoker = { m_island, m_island.StateChanged([&](
        auto&&,  
        winrt::Microsoft::UI::Content::ContentIslandStateChangedEventArgs const& args)
    {
        if (args.DidRasterizationScaleChange())
        {
            auto matrix = GetIsland().LocalToClientTransformMatrix();
            m_output.SetRasterizationTransform(Matrix2x2(matrix));
        }

        if (args.DidActualSizeChange() ||
            args.DidRasterizationScaleChange())
        {
            HandleContentLayout();
        }
    }) };

    HandleContentLayout();
}

void LiftedFrame::OnSettingChanged(SettingId id)
{
    GetOutput().GetResourceList()->OnSettingChanged(GetOutput(), id);

    if (id == Setting_DisablePixelSnapping || id == Setting_ShowPopupVisual)
    {
        HandleContentLayout();
    }
}

winrt::ChildSiteLink LiftedFrame::ConnectChildFrame(
    IFrame* frame,
    const winrt::ContainerVisual& childPlacementVisual)
{
    auto childSiteLink = winrt::ChildSiteLink::Create(m_island, childPlacementVisual);

    // If we are connecting to a system island we need to mark the link
    // appropriately. This can go away once we support input in a system island.
    if (!frame->IsLiftedFrame())
    {
        childSiteLink.AutomationOption(winrt::ContentAutomationOptions::None);
        childSiteLink.ProcessKeyboardInput(false);
        childSiteLink.ProcessPointerInput(false);
    }
    else
    {
        childSiteLink.AutomationOption(winrt::ContentAutomationOptions::FragmentBased);
    }

    childSiteLink.Connect(frame->GetIsland());

    return childSiteLink;
}

winrt::DesktopPopupSiteBridge LiftedFrame::ConnectPopupFrame(
    IFrame* frame)
{
    auto desktopPopupSiteBridge = winrt::DesktopPopupSiteBridge::Create(m_island);

    // If we are connecting to a system island we need to mark the popup
    // appropriately. This can go away once we support input in a system island.
    if (!frame->IsLiftedFrame())
    {
        // Currently, we don't support hosting SystemFrames in DesktopPopupSiteBridge.
    }
    else
    {
        desktopPopupSiteBridge.AutomationOption(winrt::ContentAutomationOptions::FragmentBased);
    }

    desktopPopupSiteBridge.Connect(frame->GetIsland());

    return desktopPopupSiteBridge;
}

void LiftedFrame::HandleContentLayout()
{
    GetOutput().GetResourceList()->EnsureInitialized(GetOutput());
    
    m_rootVisualTreeNode->Size(m_island.ActualSize());
    m_rootVisualTreeNode->ComputeSizeAndTransform();
}

void LiftedFrame::OnPreTranslateDirectMessage(
    const MSG* /*msg*/,
    UINT /*keyboardModifiers*/,
    _Inout_ bool* handled)
{
    *handled = false;
}

void LiftedFrame::OnPreTranslateTreeMessage(
    const MSG* /*msg*/,
    UINT /*keyboardModifiers*/,
    _Inout_ bool* handled)
{
    *handled = false;
}
