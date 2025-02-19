// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

#include <wil/cppwinrt.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Numerics.h>

#include "HitTestContext.h"

class FocusList;

struct VisualTreeNode : std::enable_shared_from_this<VisualTreeNode>
{
    [[nodiscard]] static std::shared_ptr<VisualTreeNode> Create(
        _In_ winrt::com_ptr<::IUnknown> const& visual) noexcept;

    explicit VisualTreeNode() noexcept = default;
    ~VisualTreeNode() noexcept = default;

    // Disable move and copy.
    explicit VisualTreeNode(VisualTreeNode const&) = delete;
    explicit VisualTreeNode(VisualTreeNode&&) = delete;
    VisualTreeNode& operator=(VisualTreeNode const&) = delete;
    VisualTreeNode& operator=(VisualTreeNode&&) = delete;

    [[nodiscard]] winrt::Windows::Foundation::Numerics::float2 Size() const noexcept;
    void Size(_In_ winrt::Windows::Foundation::Numerics::float2 const& value) noexcept;

    [[nodiscard]] winrt::Windows::Foundation::Numerics::float4x4 Transform4x4() const noexcept;
    [[nodiscard]] winrt::Windows::Foundation::Numerics::float3x2 Transform3x2() const noexcept;

    void AddChild(_In_ std::shared_ptr<VisualTreeNode> const& child);
    void RemoveChild(_In_ std::shared_ptr<VisualTreeNode> const& child);
    void RemoveAllChildren();

    void ComputeSizeAndTransform();
    [[nodiscard]] winrt::Windows::Foundation::Rect BoundsInTreeRootCoordinates() const;

    [[nodiscard]] std::shared_ptr<VisualTreeNode> HitTestInTreeRootCoordinates(
        _In_ winrt::Windows::Foundation::Point const& point);

    [[nodiscard]] bool Match(_In_ winrt::com_ptr<::IUnknown> const& visual) const noexcept;
    [[nodiscard]] winrt::com_ptr<::IUnknown> Visual() const noexcept { std::unique_lock lock{ m_mutex }; return m_visual; }

    void HitTestCallback(_In_ ::HitTestCallback const& value) noexcept { m_hitTestCallback = value; }
    [[nodiscard]] ::HitTestCallback const& HitTestCallback() const noexcept { return m_hitTestCallback; }

    void OwningFocusList(_In_ const std::weak_ptr<FocusList>& value) noexcept { m_owningFocusList = value; }
    [[nodiscard]] std::shared_ptr<FocusList> OwningFocusList() const noexcept { return m_owningFocusList.lock(); }

    std::shared_ptr<VisualTreeNode> Parent() const noexcept { return m_parent.lock(); }

    void IsBorderVisible(bool visible);

private:
    void Initialize(_In_ winrt::com_ptr<::IUnknown> const& visual);

    // Called on child visuals, so does need to hold the lock.
    void UpdateParent(_In_ std::shared_ptr<VisualTreeNode> const& value);

    [[nodiscard]] winrt::Windows::Foundation::Numerics::float2 const& SizeInternal() const noexcept;
    [[nodiscard]] winrt::Windows::Foundation::Numerics::float4x4 const& TransformInternal() const noexcept;

    void ComputeSizeAndTransformInternal();

    winrt::Windows::Foundation::Numerics::float3 VisualOffset() const noexcept;
    winrt::Windows::Foundation::Numerics::float2 VisualSize() const noexcept;
    winrt::Windows::Foundation::Numerics::float3 VisualScale() const noexcept;
    winrt::Windows::Foundation::Numerics::float3 VisualRelativeOffsetAdjustment() const noexcept;
    winrt::Windows::Foundation::Numerics::float2 VisualRelativeSizeAdjustment() const noexcept;
    winrt::Windows::Foundation::Numerics::float2 VisualAnchorPoint() const noexcept;
    winrt::Windows::Foundation::Numerics::float3 VisualCenterPoint() const noexcept;
    winrt::Windows::Foundation::Numerics::float3 VisualRotationAxis() const noexcept;
    float VisualRotationAngle() const noexcept;


    mutable std::mutex m_mutex{};

    winrt::com_ptr<::IUnknown> m_visual{ nullptr };
    winrt::com_ptr<::IUnknown> m_borderVisual{ nullptr };
    winrt::Windows::Foundation::Numerics::float2 m_size{ 0.0f, 0.0f };
    winrt::Windows::Foundation::Numerics::float4x4 m_transform{ winrt::Windows::Foundation::Numerics::float4x4::identity() };

    std::weak_ptr<VisualTreeNode> m_parent{};
    std::vector<std::shared_ptr<VisualTreeNode>> m_children{};

    ::HitTestCallback m_hitTestCallback{};
    std::weak_ptr<FocusList> m_owningFocusList{};
};
