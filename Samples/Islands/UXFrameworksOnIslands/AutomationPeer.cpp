// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"
#include "AutomationPeer.h"

// static
std::shared_ptr<AutomationPeer> AutomationPeer::Create(
    _In_ IFrame const* const owningFrame,
    _In_ std::shared_ptr<VisualTreeNode> const& visual,
    _In_ std::wstring_view const& name,
    _In_ long const& uiaControlTypeId) noexcept
{
    try
    {
        auto peer = std::make_shared<AutomationPeer>();
        peer->Initialize(owningFrame, visual, name, uiaControlTypeId);
        return peer;
    }
    catch (...) {}
    return nullptr;
}

winrt::Windows::Graphics::RectInt32 AutomationPeer::GetBoundingRectangleInScreenCoordinatesForAutomation(
        ::IUnknown const* const sender) const
{
    std::unique_lock lock{ m_mutex };

    if (sender != m_fragment.as<::IUnknown>().get())
    {
        return { 0, 0, 0, 0 };
    }

    auto boundsInFrameCoordinates = m_visual->BoundsInTreeRootCoordinates();
    auto coordinateConverter = m_owningFrameNoRef->GetConverter();
    return coordinateConverter.ConvertLocalToScreen(boundsInFrameCoordinates);
}

void AutomationPeer::HandleSetFocusForAutomation(
    ::IUnknown const* const)
{
    std::unique_lock lock{ m_mutex };

    // Do nothing.
}

winrt::com_ptr<::IRawElementProviderFragment> AutomationPeer::GetFirstChildFragmentForAutomation(
        ::IUnknown const* const sender) const
{
    std::unique_lock lock{ m_mutex };

    if (sender != m_fragment.as<::IUnknown>().get())
    {
        return nullptr;
    }

    if (nullptr != m_childContentLink)
    {
        return m_childContentLink.AutomationProvider().try_as<::IRawElementProviderFragment>();
    }

    return m_childFrameNoRef->GetAutomationProvider().as<::IRawElementProviderFragment>();
}

winrt::com_ptr<::IRawElementProviderFragment> AutomationPeer::GetLastChildFragmentForAutomation(
    ::IUnknown const* const sender) const
{
    std::unique_lock lock{ m_mutex };

    if (sender != m_fragment.as<::IUnknown>().get())
    {
        return nullptr;
    }

    if (nullptr != m_childContentLink)
    {
        return m_childContentLink.AutomationProvider().try_as<::IRawElementProviderFragment>();
    }

    return m_childFrameNoRef->GetAutomationProvider().as<::IRawElementProviderFragment>();
}

void AutomationPeer::SetChildContentLink(_In_ winrt::Microsoft::UI::Content::ChildContentLink const& childContentLink)
{
    std::unique_lock lock{ m_mutex };

    // If we're configuring this peer with an external child content link, we cannot have an external child frame set up.
    m_childFrameNoRef = nullptr;

    m_childContentLink = childContentLink;
    if (nullptr != m_childContentLink)
    {
        m_externalChildCallbackRevoker = m_fragment->SetExternalChildCallbackHandler(this);

        m_parentAutomationProviderRequestedRevoker = { m_childContentLink, m_childContentLink.ParentAutomationProviderRequested([&](auto&&, auto&& args)
        {
            winrt::Windows::Foundation::IInspectable provider{ nullptr };
            winrt::copy_from_abi(provider, m_fragment.as<::IInspectable>().get());
            args.AutomationProvider(provider);
            args.Handled(true);
        }) };

        m_previousSiblingAutomationProviderRequestedRevoker = { m_childContentLink, m_childContentLink.PreviousSiblingAutomationProviderRequested([&](auto&&, auto&& args)
        {
            args.AutomationProvider(nullptr);
            args.Handled(true);
        }) };

        m_nextSiblingAutomationProviderRequestedRevoker = { m_childContentLink, m_childContentLink.NextSiblingAutomationProviderRequested([&](auto&&, auto&& args)
        {
            args.AutomationProvider(nullptr);
            args.Handled(true);
        }) };

        m_fragmentRootAutomationProviderRequestedRevoker = { m_childContentLink, m_childContentLink.FragmentRootAutomationProviderRequested([&](auto&&, auto&& args)
        {
            winrt::Windows::Foundation::IInspectable provider{ nullptr };
            winrt::com_ptr<::IRawElementProviderFragmentRoot> fragmentRoot{ nullptr };
            winrt::check_hresult(m_fragment->get_FragmentRoot(fragmentRoot.put()));
            winrt::copy_from_abi(provider, fragmentRoot.as<::IInspectable>().get());
            args.AutomationProvider(provider);
            args.Handled(true);
        }) };
    }
    else
    {
        m_externalChildCallbackRevoker.reset();

        m_parentAutomationProviderRequestedRevoker.Revoke();
        m_previousSiblingAutomationProviderRequestedRevoker.Revoke();
        m_nextSiblingAutomationProviderRequestedRevoker.Revoke();
        m_fragmentRootAutomationProviderRequestedRevoker.Revoke();
    }
}

void AutomationPeer::SetChildFrame(_In_ IFrame const* const childFrame)
{
    std::unique_lock lock{ m_mutex };

    // If we're configuring this peer with an external child frame, we cannot have an external child content link set up.
    m_parentAutomationProviderRequestedRevoker.Revoke();
    m_previousSiblingAutomationProviderRequestedRevoker.Revoke();
    m_nextSiblingAutomationProviderRequestedRevoker.Revoke();
    m_fragmentRootAutomationProviderRequestedRevoker.Revoke();
    m_childContentLink = nullptr;

    m_childFrameNoRef = childFrame;
    if (nullptr != m_childFrameNoRef)
    {
        m_externalChildCallbackRevoker = m_fragment->SetExternalChildCallbackHandler(this);
    }
    else
    {
        m_externalChildCallbackRevoker.reset();
    }
}

winrt::com_ptr<::IRawElementProviderFragment> AutomationPeer::ForwardFragmentFromPointInScreenCoordinatesRequest(double x, double y) const
{
    std::unique_lock lock{ m_mutex };

    winrt::com_ptr<::IRawElementProviderFragmentRoot> childFragmentRoot{ nullptr };

    if (nullptr != m_childContentLink)
    {
        childFragmentRoot = m_childContentLink.AutomationProvider().try_as<::IRawElementProviderFragmentRoot>();
    }
    else if (nullptr != m_childFrameNoRef)
    {
        childFragmentRoot = m_childFrameNoRef->GetAutomationProvider();
    }

    if (nullptr != childFragmentRoot)
    {
        // Forward the request to the child frame or content link.
        winrt::com_ptr<::IRawElementProviderFragment> fragmentFromChildFragmentRoot{ nullptr };
        if (S_OK == childFragmentRoot->ElementProviderFromPoint(x, y, fragmentFromChildFragmentRoot.put()))
        {
            return fragmentFromChildFragmentRoot;
        }
    }

    // Return ourselves if we have no external child content or no fragment was found in the child content.
    return m_fragment;
}

void AutomationPeer::Initialize(
    _In_ IFrame const* const owningFrame,
    _In_ std::shared_ptr<VisualTreeNode> const& visual,
    _In_ std::wstring_view const& name,
    _In_ long const& uiaControlTypeId)
{
    m_owningFrameNoRef = owningFrame;
    m_visual = visual;

    m_fragment = winrt::make_self<AutomationHelpers::AutomationFragment>();
    m_fragment->Name(name);
    m_fragment->UiaControlTypeId(uiaControlTypeId);
    m_fragmentCallbackRevoker = m_fragment->SetFragmentCallbackHandler(this);
}
