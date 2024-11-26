// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"
#include "VisualTreeNode.h"

// static
std::shared_ptr<VisualTreeNode> VisualTreeNode::Create(
    _In_ winrt::com_ptr<::IUnknown> const& visual) noexcept
{
    try
    {
        auto node = std::make_shared<VisualTreeNode>();
        node->Initialize(visual);
        return node;
    }
    catch (...) {}
    return nullptr;
}

winrt::Windows::Foundation::Numerics::float2 VisualTreeNode::Size() const noexcept
{
    std::unique_lock lock{ m_mutex };
    return SizeInternal();
}

void VisualTreeNode::Size(_In_ winrt::Windows::Foundation::Numerics::float2 const& value) noexcept
{
    std::unique_lock lock{ m_mutex };
    m_size = value;
}

winrt::Windows::Foundation::Numerics::float4x4 VisualTreeNode::Transform4x4() const noexcept
{
    std::unique_lock lock{ m_mutex };
    return TransformInternal();
}

winrt::Windows::Foundation::Numerics::float3x2 VisualTreeNode::Transform3x2() const noexcept
{
    std::unique_lock lock{ m_mutex };

    auto t = TransformInternal();
    winrt::Windows::Foundation::Numerics::float3x2 ct{ t.m11, t.m12, t.m21, t.m22, t.m41, t.m42};

    return ct;
}

void VisualTreeNode::AddChild(_In_ std::shared_ptr<VisualTreeNode> const& child)
{
    std::unique_lock lock{ m_mutex };

    auto childVisual = child->Visual();
    bool childVisualAdded{ false };

    if (auto parent = m_visual.try_as<winrt::Windows::UI::Composition::ContainerVisual>())
    {
        parent.Children().InsertAtTop(childVisual.as<winrt::Windows::UI::Composition::Visual>());
        childVisualAdded = true;
    }
    else if (auto parent2 = m_visual.try_as<winrt::Microsoft::UI::Composition::ContainerVisual>())
    {
        parent2.Children().InsertAtTop(childVisual.as<winrt::Microsoft::UI::Composition::Visual>());
        childVisualAdded = true;
    }

    if (childVisualAdded)
    {
        child->UpdateParent(shared_from_this());
        m_children.push_back(child);

        ComputeSizeAndTransformInternal();
    }
}

void VisualTreeNode::RemoveChild(_In_ std::shared_ptr<VisualTreeNode> const& child)
{
    std::unique_lock lock{ m_mutex };

    auto it = std::find(m_children.begin(), m_children.end(), child);
    if (it != m_children.end())
    {
        auto childVisual = child->Visual();

        if (auto parent = m_visual.try_as<winrt::Windows::UI::Composition::ContainerVisual>())
        {
            parent.Children().Remove(childVisual.as<winrt::Windows::UI::Composition::Visual>());
        }
        else if (auto parent2 = m_visual.try_as<winrt::Microsoft::UI::Composition::ContainerVisual>())
        {
            parent2.Children().Remove(childVisual.as<winrt::Microsoft::UI::Composition::Visual>());
        }

        child->UpdateParent(nullptr);

        m_children.erase(it);
    }
}

void VisualTreeNode::RemoveAllChildren()
{
    std::unique_lock lock{ m_mutex };

    if (auto parent = m_visual.try_as<winrt::Windows::UI::Composition::ContainerVisual>())
    {
        parent.Children().RemoveAll();
    }
    else if (auto parent2 = m_visual.try_as<winrt::Microsoft::UI::Composition::ContainerVisual>())
    {
        parent2.Children().RemoveAll();
    }

    for (auto& child : m_children)
    {
        child->UpdateParent(nullptr);
    }

    m_children.clear();
}

void VisualTreeNode::ComputeSizeAndTransform()
{
    std::unique_lock lock{ m_mutex };

    ComputeSizeAndTransformInternal();
}

winrt::Windows::Foundation::Rect VisualTreeNode::BoundsInTreeRootCoordinates() const
{
    std::unique_lock lock{ m_mutex };

    // Local bound components.
    auto topLeft = winrt::Windows::Foundation::Numerics::float2{ 0.0f, 0.0f };
    auto topRight = winrt::Windows::Foundation::Numerics::float2{ m_size.x, 0.0f };
    auto bottomLeft = winrt::Windows::Foundation::Numerics::float2{ 0.0f, m_size.y };
    auto bottomRight = winrt::Windows::Foundation::Numerics::float2{ m_size.x, m_size.y };

    // Transform the local bounds to the root visual's coordinate space.
    auto topLeftTransformed = winrt::Windows::Foundation::Numerics::transform(topLeft, m_transform);
    auto topRightTransformed = winrt::Windows::Foundation::Numerics::transform(topRight, m_transform);
    auto bottomLeftTransformed = winrt::Windows::Foundation::Numerics::transform(bottomLeft, m_transform);
    auto bottomRightTransformed = winrt::Windows::Foundation::Numerics::transform(bottomRight, m_transform);

    // Compute the bounding box of the transformed local bounds.
    auto left = std::min({ topLeftTransformed.x, topRightTransformed.x, bottomLeftTransformed.x, bottomRightTransformed.x });
    auto top = std::min({ topLeftTransformed.y, topRightTransformed.y, bottomLeftTransformed.y, bottomRightTransformed.y });
    auto right = std::max({ topLeftTransformed.x, topRightTransformed.x, bottomLeftTransformed.x, bottomRightTransformed.x });
    auto bottom = std::max({ topLeftTransformed.y, topRightTransformed.y, bottomLeftTransformed.y, bottomRightTransformed.y });

    return winrt::Windows::Foundation::Rect{ left, top, right - left, bottom - top };
}

std::shared_ptr<VisualTreeNode> VisualTreeNode::HitTestInTreeRootCoordinates(
    _In_ winrt::Windows::Foundation::Point const& point)
{
    std::unique_lock lock{ m_mutex };

    auto pointInTreeCoordinates = winrt::Windows::Foundation::Numerics::float2{ point.X, point.Y };

    // Transform the point to the local coordinate space.
    winrt::Windows::Foundation::Numerics::float4x4 inverseTransform;
    if (!winrt::Windows::Foundation::Numerics::invert(m_transform, &inverseTransform))
    {
        // For now, hit testing is not supported if the transform is not invertible.
        return nullptr;
    }

    auto pointInLocalCoordinates = winrt::Windows::Foundation::Numerics::transform(pointInTreeCoordinates, inverseTransform);
    if (pointInLocalCoordinates.x < 0.0f || pointInLocalCoordinates.x >= m_size.x ||
        pointInLocalCoordinates.y < 0.0f || pointInLocalCoordinates.y >= m_size.y)
    {
        // Out of bounds.
        return nullptr;
    }

    // Check if the point is within any of the children.
    // Iterate in reverse to obey the z-order.
    for (auto it = m_children.rbegin(); it != m_children.rend(); ++it)
    {
        auto child = *it;
        auto childHit = child->HitTestInTreeRootCoordinates(point);
        if (nullptr != childHit)
        {
            return childHit;
        }
    }

    // If none of the children were hit, then return this visual.
    return shared_from_this();
}

bool VisualTreeNode::Match(_In_ winrt::com_ptr<::IUnknown> const& visual) const noexcept
{
    std::unique_lock lock{ m_mutex };
    return m_visual.get() == visual.get();
}

void VisualTreeNode::Initialize(_In_ winrt::com_ptr<::IUnknown> const& visual)
{
    m_visual = visual;
}

void VisualTreeNode::UpdateParent(_In_ std::shared_ptr<VisualTreeNode> const& value)
{
    // This is private, but needs to take the lock since it is only called on child VisualTreeNodes.
    // See usage in AddChild, RemoveChild, and RemoveAllChildren.
    std::unique_lock lock{ m_mutex };
    m_parent = value;
}

winrt::Windows::Foundation::Numerics::float2 const& VisualTreeNode::SizeInternal() const noexcept
{
    return m_size;
}

winrt::Windows::Foundation::Numerics::float4x4 const& VisualTreeNode::TransformInternal() const noexcept
{
    return m_transform;
}

void VisualTreeNode::ComputeSizeAndTransformInternal()
{
    if (auto strongParent = m_parent.lock())
    {
        // Compute the local size of this visual.
        auto parentSize = strongParent->SizeInternal();
        auto visualSize = VisualSize();
        auto visualRelativeSizeAdjustment = VisualRelativeSizeAdjustment();
        m_size = {
            visualSize.x + (visualRelativeSizeAdjustment.x * parentSize.x),
            visualSize.y + (visualRelativeSizeAdjustment.y * parentSize.y) };

        // Compute the local transform of this visual.
        auto visualAnchorPoint = VisualAnchorPoint();
        winrt::Windows::Foundation::Numerics::float3 anchorPointOffset{
            visualAnchorPoint.x * m_size.x,
            visualAnchorPoint.y * m_size.y,
            0.0f };
        auto visualOffset = VisualOffset();
        auto visualRelativeOffsetAdjustment = VisualRelativeOffsetAdjustment();
        winrt::Windows::Foundation::Numerics::float3 localOffset{
            visualOffset.x + (visualRelativeOffsetAdjustment.x * parentSize.x),
            visualOffset.y + (visualRelativeOffsetAdjustment.y * parentSize.y),
            visualOffset.z };
        auto translation = winrt::Windows::Foundation::Numerics::make_float4x4_translation(localOffset - anchorPointOffset);

        auto visualCenterPoint = VisualCenterPoint();
        auto visualScale = VisualScale();
        auto scale = winrt::Windows::Foundation::Numerics::make_float4x4_scale(visualScale, visualCenterPoint);

        auto visualRotationAxis = VisualRotationAxis();
        auto visualRotationAngle = VisualRotationAngle();
        auto rotationWithoutCenterPoint = winrt::Windows::Foundation::Numerics::make_float4x4_from_axis_angle(visualRotationAxis, visualRotationAngle);
        auto rotation =
            winrt::Windows::Foundation::Numerics::make_float4x4_translation(-visualCenterPoint) *
            rotationWithoutCenterPoint *
            winrt::Windows::Foundation::Numerics::make_float4x4_translation(visualCenterPoint);

        auto localTransform = rotation * scale * translation;
        m_transform = localTransform * strongParent->TransformInternal();
    }

    // Recursively compute the size and transform of all children.
    for (auto& child : m_children)
    {
        child->ComputeSizeAndTransformInternal();
    }
}

winrt::Windows::Foundation::Numerics::float3 VisualTreeNode::VisualOffset() const noexcept
{
    winrt::Windows::Foundation::Numerics::float3 offset{ 0.0f, 0.0f, 0.0f };

    if (auto visual = m_visual.try_as<winrt::Windows::UI::Composition::Visual>())
    {
        offset = visual.Offset();
    }
    else if (auto visual2 = m_visual.try_as<winrt::Microsoft::UI::Composition::Visual>())
    {
        offset = visual2.Offset();
    }

    return offset;
}

winrt::Windows::Foundation::Numerics::float2 VisualTreeNode::VisualSize() const noexcept
{
    winrt::Windows::Foundation::Numerics::float2 size{ 0.0f, 0.0f };

    if (auto visual = m_visual.try_as<winrt::Windows::UI::Composition::Visual>())
    {
        size = visual.Size();
    }
    else if (auto visual2 = m_visual.try_as<winrt::Microsoft::UI::Composition::Visual>())
    {
        size = visual2.Size();
    }

    return size;
}

winrt::Windows::Foundation::Numerics::float3 VisualTreeNode::VisualScale() const noexcept
{
    winrt::Windows::Foundation::Numerics::float3 scale{ 1.0f, 1.0f, 1.0f };

    if (auto visual = m_visual.try_as<winrt::Windows::UI::Composition::Visual>())
    {
        scale = visual.Scale();
    }
    else if (auto visual2 = m_visual.try_as<winrt::Microsoft::UI::Composition::Visual>())
    {
        scale = visual2.Scale();
    }

    return scale;
}

winrt::Windows::Foundation::Numerics::float3 VisualTreeNode::VisualRelativeOffsetAdjustment() const noexcept
{
    winrt::Windows::Foundation::Numerics::float3 relativeOffsetAdjustment{ 0.0f, 0.0f, 0.0f };

    if (auto visual = m_visual.try_as<winrt::Windows::UI::Composition::Visual>())
    {
        relativeOffsetAdjustment = visual.RelativeOffsetAdjustment();
    }
    else if (auto visual2 = m_visual.try_as<winrt::Microsoft::UI::Composition::Visual>())
    {
        relativeOffsetAdjustment = visual2.RelativeOffsetAdjustment();
    }

    return relativeOffsetAdjustment;
}

winrt::Windows::Foundation::Numerics::float2 VisualTreeNode::VisualRelativeSizeAdjustment() const noexcept
{
    winrt::Windows::Foundation::Numerics::float2 relativeSizeAdjustment{ 0.0f, 0.0f };

    if (auto visual = m_visual.try_as<winrt::Windows::UI::Composition::Visual>())
    {
        relativeSizeAdjustment = visual.RelativeSizeAdjustment();
    }
    else if (auto visual2 = m_visual.try_as<winrt::Microsoft::UI::Composition::Visual>())
    {
        relativeSizeAdjustment = visual2.RelativeSizeAdjustment();
    }

    return relativeSizeAdjustment;
}

winrt::Windows::Foundation::Numerics::float2 VisualTreeNode::VisualAnchorPoint() const noexcept
{
    winrt::Windows::Foundation::Numerics::float2 anchorPoint{ 0.0f, 0.0f };

    if (auto visual = m_visual.try_as<winrt::Windows::UI::Composition::Visual>())
    {
        anchorPoint = visual.AnchorPoint();
    }
    else if (auto visual2 = m_visual.try_as<winrt::Microsoft::UI::Composition::Visual>())
    {
        anchorPoint = visual2.AnchorPoint();
    }

    return anchorPoint;
}

winrt::Windows::Foundation::Numerics::float3 VisualTreeNode::VisualCenterPoint() const noexcept
{
    winrt::Windows::Foundation::Numerics::float3 centerPoint{ 0.0f, 0.0f, 0.0f };

    if (auto visual = m_visual.try_as<winrt::Windows::UI::Composition::Visual>())
    {
        centerPoint = visual.CenterPoint();
    }
    else if (auto visual2 = m_visual.try_as<winrt::Microsoft::UI::Composition::Visual>())
    {
        centerPoint = visual2.CenterPoint();
    }

    return centerPoint;
}

winrt::Windows::Foundation::Numerics::float3 VisualTreeNode::VisualRotationAxis() const noexcept
{
    winrt::Windows::Foundation::Numerics::float3 rotationAxis{ 0.0f, 0.0f, 1.0f };

    if (auto visual = m_visual.try_as<winrt::Windows::UI::Composition::Visual>())
    {
        rotationAxis = visual.RotationAxis();
    }
    else if (auto visual2 = m_visual.try_as<winrt::Microsoft::UI::Composition::Visual>())
    {
        rotationAxis = visual2.RotationAxis();
    }

    return rotationAxis;
}

float VisualTreeNode::VisualRotationAngle() const noexcept
{
    float rotationAngle = 0.0f;

    if (auto visual = m_visual.try_as<winrt::Windows::UI::Composition::Visual>())
    {
        rotationAngle = visual.RotationAngle();
    }
    else if (auto visual2 = m_visual.try_as<winrt::Microsoft::UI::Composition::Visual>())
    {
        rotationAngle = visual2.RotationAngle();
    }

    return rotationAngle;
}
