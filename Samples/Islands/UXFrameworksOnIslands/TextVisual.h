// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

#include "OutputResource.h"
#include "D2DSprite.h"
#include "AutomationTree.h"
#include "VisualTreeNode.h"

// Object that encapsulates a sprite visual containing text, which is
// rendered to a composition surface.
template<class T>
class TextVisual final : public D2DSprite<T>
{
public:
    TextVisual(Output<T> const& output, std::wstring&& text);

    TextVisual(
        Output<T> const& output,
        std::wstring&& text,
        winrt::com_ptr<IDWriteTextLayout> const& textLayout,
        ContainerVisual const& containerVisual = nullptr);

    auto& GetText() const noexcept
    {
        return m_text;
    }

    winrt::Size Size() const
    {
        return m_size;
    }

    winrt::Windows::Foundation::Numerics::float2 GetTextOrigin() const
    {
        return m_origin;
    }

    void SetTextOrigin(winrt::Windows::Foundation::Numerics::float2 origin);

    winrt::Windows::UI::Color const& GetTextColor() const&
    {
        return m_textColor;
    }

    void SetTextColor(winrt::Windows::UI::Color const& color);

    void SetFormatWidth(float width);

protected:
    // D2DVisual methods.
    D2D1_RECT_F GetPixelBounds(Output<T> const& output, D2D1::Matrix3x2F const& rasterTransform) override;
    void RenderContent(Output<T> const& output, ID2D1DeviceContext5* deviceContext) override;

private:
    void SetVisualSize();
    D2D_RECT_F GetLogicalBounds() const noexcept;

    std::wstring m_text;
    winrt::com_ptr<IDWriteTextLayout> m_textLayout;
    winrt::Windows::UI::Color m_textColor = winrt::Windows::UI::Colors::Black();
    winrt::Size m_size = {};
    winrt::Windows::Foundation::Numerics::float2 m_origin = {};
};

using LiftedTextVisual = TextVisual<winrt::Compositor>;
using SystemTextVisual = TextVisual<winrt::WUC::Compositor>;

// Inserts a TextVisual into a visual tree.
template <class T>
void InsertTextVisual(
    TextVisual<T> const& textVisual,
    std::shared_ptr<AutomationTree> const& automationTree,
    std::shared_ptr<AutomationPeer> const& parentPeer
    )
{
    auto visualNode = VisualTreeNode::Create(textVisual.GetVisual().as<IUnknown>());
    auto visualPeer = automationTree->CreatePeer(visualNode, textVisual.GetText(), UIA_TextControlTypeId);
    parentPeer->VisualNode()->AddChild(visualNode);
    parentPeer->Fragment()->AddChildToEnd(visualPeer->Fragment());
}
