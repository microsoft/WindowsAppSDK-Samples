// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"
#include "WebViewFrame.h"
#include "ColorUtils.h"
#include "VisualUtils.h"
#include "TextRenderer.h"
#include "FocusManager.h"

WebViewFrame::WebViewFrame(
        const winrt::DispatcherQueue& queue,
        const winrt::WUC::Compositor& systemCompositor,
        const std::shared_ptr<SettingCollection>& settings) :
    SystemFrame(queue, systemCompositor, settings)
{
    InitializeVisualTree(systemCompositor);
}

bool WebViewFrame::SystemPreTranslateMessage(
    UINT message,
    WPARAM wParam,
    LPARAM /*lParam*/)
{
    if (message == WM_SYSKEYDOWN || message == WM_KEYDOWN)
    {
        if (static_cast<winrt::Windows::System::VirtualKey>(wParam) == winrt::Windows::System::VirtualKey::Menu)
        {
            m_acceleratorActive = !m_acceleratorActive;
            m_acceleratorVisual->GetVisual().IsVisible(m_acceleratorActive);
            HandleContentLayout();
        }
        else {
            m_acceleratorActive = false;
            m_acceleratorVisual->GetVisual().IsVisible(false);
            if (static_cast<winrt::Windows::System::VirtualKey>(wParam) == winrt::Windows::System::VirtualKey::Number5)
            {
                // Take focus
                GetFocusList()->GetManager()->SetFocusToVisual(GetRootVisualTreeNode(), GetRootVisualTreeNode()->OwningFocusList());
            }
            HandleContentLayout();
        }
    }
    return false;
}

void WebViewFrame::InitializeVisualTree(
    const winrt::WUC::Compositor& compositor)
{
    // Fill entire bounds with light brown color
    auto colorVisual = compositor.CreateSpriteVisual();
    auto colorVisualNode = VisualTreeNode::Create(colorVisual.as<::IUnknown>());
    m_colorVisualPeer = m_automationTree->CreatePeer(colorVisualNode, k_frameName, UIA_ImageControlTypeId);
    GetRootVisualTreeNode()->AddChild(colorVisualNode);
    m_automationTree->Root()->AddChildToEnd(m_colorVisualPeer->Fragment());
    m_automationTree->AddPeer(m_colorVisualPeer);
    VisualUtils::FillWithColor(colorVisual, ColorUtils::LightBrown());

    // Create the text format objects used for content.
    auto headingTextFormat = CreateTextFormat(L"Segoe UI", k_headingFontSize, DWRITE_FONT_WEIGHT_BOLD);
    auto bodyTextFormat = CreateTextFormat(L"Segoe UI");
    bodyTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
    auto codeTextFormat = CreateTextFormat(L"Consolas");

    // Add the content.
    m_content.reserve(6);
    AddContent(L"WebView", nullptr);
    m_acceleratorVisual = AddContent(L"(Hotkey: 5)", nullptr);
    m_acceleratorVisual->GetVisual().IsVisible(false);
    AddContent(L"UXFrameworksOnIslands Sample", headingTextFormat.get());
    AddContent(
        L"This sample application demonstrates how multiple UX frameworks could be a used in the "
        L"same application by hosting each framework in its own content island. This application "
        L"does not actually use any UX frameworks; it merely demonstrates nested content islands.",
        bodyTextFormat.get());
    AddContent(L"Frame Hierarchy", headingTextFormat.get());
    AddContent(
        L"Below is the hierarchy of frames (i.e., islands) in this application.",
        bodyTextFormat.get());
    AddContent(
        L"Root (System)\n"
        L" \x2502\n"
        L" \x251C\x2500\x2500 NetUI (System)\n"
        L" \x2502     \x2502\n"
        L" \x2502     \x2514\x2500\x2500 ReactNative (Lifted)\n"
        L" \x2502           \x2502\n"
        L" \x2502           \x2514\x2500\x2500 WinUI (Lifted)\n"
        L" \x2502\n"
        L" \x2514\x2500\x2500 WebView (System)\n",
        codeTextFormat.get());
}

std::shared_ptr<SystemTextVisual> WebViewFrame::AddContent(_In_z_ WCHAR const* text, IDWriteTextFormat* textFormat)
{
    auto textLayout = CreateTextLayout(text, textFormat);
    auto block = std::make_shared<SystemTextVisual>(GetOutput(), text, textLayout);
    m_content.push_back(block);
    InsertTextVisual(*m_content.back(), m_automationTree, m_colorVisualPeer);
    return block;
}

void WebViewFrame::HandleContentLayout()
{
    float islandWidth = GetIsland().ActualSize().x;
    float columnWidth = std::max(islandWidth - (k_leftMargin + k_rightMargin), k_minColumNWidth);

    float y = k_topMargin;

    for (auto& block : m_content)
    {
        if (block->IsVisible())
        {
            block->SetFormatWidth(columnWidth);
    
            auto& visual = block->GetVisual();
            visual.Offset({k_leftMargin, y, 0.0f});
            y += visual.Size().y + k_paraGap;
        }
    }

    SystemFrame::HandleContentLayout();
}
