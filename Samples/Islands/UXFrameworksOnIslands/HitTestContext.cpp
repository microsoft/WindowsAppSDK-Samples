// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"
#include "HitTestContext.h"

#include "FocusList.h"
#include "FocusManager.h"
#include "VisualTreeNode.h"

HitTestContext::HitTestContext(
    FocusManager& focusManager,
    const std::shared_ptr<VisualTreeNode>& rootNode) :
    m_focusManager(focusManager),
    m_rootNode(rootNode)
{
}

void HitTestContext::RunHitTest(
    const winrt::Point& point,
    bool isRightClick)
{
    m_isRightClick = isRightClick;

    // Ask the VisualTreeNode to hittest the point down to a visual.
    auto hitTestResult = m_rootNode->HitTestInTreeRootCoordinates(point);

    // Walk up the tree to find the first node that has a callback, and the first node that is
    // focusable.
    auto currentNode = hitTestResult;
    while (currentNode != nullptr)
    {
        if (!m_foundCallbackNode)
        {
            if (auto& callback = currentNode->HitTestCallback())
            {
                std::optional<winrt::Point> localPoint = TryConvertToLocalPoint(
                    currentNode->Transform4x4(),
                    point);
                if (localPoint.has_value())
                {
                    m_point = *localPoint;
                    bool handled = callback(*this);

                    if (handled)
                    {
                        m_foundCallbackNode = true;
                    }
                }
            }
        }

        if (!m_foundFocusNode)
        {
            auto focusList = currentNode->OwningFocusList();
            if (focusList != nullptr)
            {
                m_focusManager.SetFocusFromClick(currentNode, focusList);
                m_foundFocusNode = true;
            }
        }

        if (m_foundCallbackNode && m_foundFocusNode)
        {
            break;
        }

        currentNode = currentNode->Parent();
    }
}

/*static*/
void HitTestContext::ConfigureCrossTreeConnection(
    const std::shared_ptr<VisualTreeNode>& parent,
    const std::shared_ptr<VisualTreeNode>& child)
{
    parent->HitTestCallback([child](HitTestContext& context) {
        return context.ForwardToCrossTree(child);
    });
}

bool HitTestContext::ForwardToCrossTree(
    const std::shared_ptr<VisualTreeNode>& childNode)
{
    HitTestContext childContext{ m_focusManager, childNode };
    childContext.m_foundCallbackNode = m_foundCallbackNode;
    childContext.m_foundFocusNode = m_foundFocusNode;

    // m_point is already in child frame's coordinates
    childContext.RunHitTest(m_point, m_isRightClick);

    m_foundCallbackNode = childContext.m_foundCallbackNode;
    m_foundFocusNode = childContext.m_foundFocusNode;

    return m_foundCallbackNode;
}

/*static*/
std::optional<winrt::Point> HitTestContext::TryConvertToLocalPoint(
    const winrt::float4x4& localToRootTransform,
    const winrt::Point& point)
{
    winrt::float4x4 rootToLocalTransform;
    bool success = winrt::invert(localToRootTransform, &rootToLocalTransform);
    if (!success)
    {
        return std::nullopt;
    }

    winrt::float2 rootPoint = winrt::float2{ point.X, point.Y };
    winrt::float2 localPoint = winrt::transform(rootPoint, rootToLocalTransform);

    return winrt::Point{ localPoint.x, localPoint.y };
}
