// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"
#include "LiftedFrame.h"

LiftedFrame::LiftedFrame(const winrt::Compositor& compositor) : m_output(compositor)
{
    auto rootVisual = compositor.CreateContainerVisual();
    rootVisual.RelativeSizeAdjustment({1.0f, 1.0f});
    m_rootVisualTreeNode = VisualTreeNode::Create(rootVisual.as<::IUnknown>());
    m_island = winrt::ContentIsland::Create(rootVisual);
    m_automationTree = AutomationTree::Create(this, L"LiftedFrame", UIA_PaneControlTypeId);

    // Set up a handler for size changes
    m_islandStateChangedRevoker = { m_island, m_island.StateChanged([&](auto&&,  winrt::Microsoft::UI::Content::ContentIslandStateChangedEventArgs const& args)
    {
        if (args.DidRasterizationScaleChange())
        {
            m_output.SetRasterizationScale(GetIsland().RasterizationScale());
        }

        if (args.DidActualSizeChange() ||
            args.DidRasterizationScaleChange())
        {
            HandleContentLayout();
        }
    }) };

    HandleContentLayout();
}

winrt::ChildContentLink LiftedFrame::ConnectChildFrame(
    IFrame* frame,
    const winrt::ContainerVisual& childPlacementVisual)
{
    auto childLink = winrt::ChildContentLink::Create(
        m_island,
        childPlacementVisual);

    // If we are connecting to a system island we need to mark the link
    // appropriately. This can go away once we support input in a system island.
    if (!frame->IsLiftedFrame())
    {
        childLink.SetIsInputPassThrough(true);
        childLink.AutomationOption(winrt::AutomationOptions::Disabled);
    }
    else
    {
        childLink.AutomationOption(winrt::AutomationOptions::NavigatableFragment);
    }

    childLink.Connect(frame->GetIsland());

    return childLink;
}

void LiftedFrame::HandleContentLayout()
{
    m_rootVisualTreeNode->Size(m_island.ActualSize());
    m_rootVisualTreeNode->ComputeSizeAndTransform();
}
