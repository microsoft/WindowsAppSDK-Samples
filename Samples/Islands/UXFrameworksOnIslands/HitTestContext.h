// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

struct VisualTreeNode;
class FocusManager;

class HitTestContext
{
public:
    HitTestContext(
        FocusManager& focusManager,
        const std::shared_ptr<VisualTreeNode>& rootNode);

    void RunHitTest(const winrt::Point& point, bool isRightClick);

    const winrt::Point& Point() const noexcept { return m_point; }

    bool IsRightClick() const noexcept { return m_isRightClick; }

    // Sets up a hit test connection between two nodes in different trees.
    // The hit test will act as if the child node is a child of the parent node.
    static void ConfigureCrossTreeConnection(
        const std::shared_ptr<VisualTreeNode>& parent,
        const std::shared_ptr<VisualTreeNode>& child);

private:
    bool ForwardToCrossTree(
        const std::shared_ptr<VisualTreeNode>& childNode);

    // Returns nullopt if a conversion can't be done - for example the transform isn't invertible.
    static std::optional<winrt::Point> TryConvertToLocalPoint(
        const winrt::float4x4& localToRootTransform,
        const winrt::Point& point);

    winrt::Point m_point;
    FocusManager& m_focusManager;
    std::shared_ptr<VisualTreeNode> m_rootNode;
    bool m_isRightClick = false;
    bool m_foundCallbackNode = false;
    bool m_foundFocusNode = false;
};

using HitTestCallback = std::function<bool(HitTestContext&)>;
