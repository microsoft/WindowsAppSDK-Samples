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
    TextVisual(
        Output<T> const& output,
        std::wstring&& text,
        winrt::Windows::UI::Color const& backgroundColor = winrt::Windows::UI::Colors::Transparent(),
        winrt::Windows::UI::Color const& textColor = winrt::Windows::UI::Colors::Black(),
        IDWriteTextFormat* textFormat = nullptr);

    auto& GetText() const noexcept
    {
        return m_text;
    }

    winrt::Size Measure() const;

protected:
    // D2DVisual methods.
    D2D1_RECT_F GetPixelBounds(Output<T> const& output, D2D1::Matrix3x2F const& rasterTransform) override;
    void RenderContent(ID2D1DeviceContext5* deviceContext) override;

private:
    std::wstring m_text;
    winrt::com_ptr<IDWriteTextLayout> m_textLayout;
    winrt::Windows::UI::Color m_backgroundColor;
    winrt::Windows::UI::Color m_textColor;
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
    auto labelVisualNode = VisualTreeNode::Create(textVisual.GetVisual().as<IUnknown>());
    auto labelVisualPeer = automationTree->CreatePeer(labelVisualNode, textVisual.GetText(), UIA_TextControlTypeId);
    parentPeer->VisualNode()->AddChild(labelVisualNode);
    parentPeer->Fragment()->AddChildToEnd(labelVisualPeer->Fragment());
}
