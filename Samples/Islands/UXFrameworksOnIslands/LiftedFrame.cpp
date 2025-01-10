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

    if (id == Setting_DisablePixelSnapping)
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
        childSiteLink.InputCapabilities(winrt::InputCapabilities::None);
        childSiteLink.AutomationTreeOption(winrt::AutomationTreeOptions::None);
    }
    else
    {
        childSiteLink.AutomationTreeOption(winrt::AutomationTreeOptions::FragmentBased);
    }

    childSiteLink.Connect(frame->GetIsland());

    return childSiteLink;
}

void LiftedFrame::HandleContentLayout()
{
    GetOutput().GetResourceList()->EnsureInitialized(GetOutput());
    
    m_rootVisualTreeNode->Size(m_island.ActualSize());
    m_rootVisualTreeNode->ComputeSizeAndTransform();
}
