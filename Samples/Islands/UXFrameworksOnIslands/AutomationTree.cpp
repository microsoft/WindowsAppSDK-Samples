// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"
#include "AutomationTree.h"

// static
std::shared_ptr<AutomationTree> AutomationTree::Create(
    _In_ IFrame const* const owningFrame,
    _In_ std::wstring_view const& name,
    _In_ long const& uiaControlTypeId) noexcept
{
    try
    {
        auto tree = std::make_shared<AutomationTree>();
        tree->Initialize(owningFrame, name, uiaControlTypeId);
        return tree;
    }
    catch (...) {}
    return nullptr;
}

winrt::Windows::Graphics::RectInt32 AutomationTree::GetBoundingRectangleInScreenCoordinatesForAutomation(
    ::IUnknown const* const sender) const
{
    std::unique_lock lock{ m_mutex };

    if (sender != m_root.as<::IUnknown>().get())
    {
        return { 0, 0, 0, 0 };
    }

    auto boundsInFrameCoordinates = m_owningFrameNoRef->GetRootVisualTreeNode()->BoundsInTreeRootCoordinates();
    auto coordinateConverter = m_owningFrameNoRef->GetConverter();
    return coordinateConverter.ConvertLocalToScreen(boundsInFrameCoordinates);
}

void AutomationTree::HandleSetFocusForAutomation(
    ::IUnknown const* const)
{
    std::unique_lock lock{ m_mutex };

    // Do nothing.
}

winrt::com_ptr<::IRawElementProviderFragment> AutomationTree::GetFragmentFromPointInScreenCoordinatesForAutomation(
    double x,
    double y,
    ::IUnknown const* const sender) const
{
    std::unique_lock lock{ m_mutex };

    if (sender != m_root.as<::IUnknown>().get())
    {
        return nullptr;
    }

    auto rootVisual = m_owningFrameNoRef->GetRootVisualTreeNode();
    auto coordinateConverter = m_owningFrameNoRef->GetConverter();
    winrt::Windows::Graphics::PointInt32 pointInScreenCoordinates{ static_cast<int32_t>(x + 0.5), static_cast<int32_t>(y + 0.5) };
    auto pointInFrameCoordinates = coordinateConverter.ConvertScreenToLocal(pointInScreenCoordinates);
    auto visualHit = rootVisual->HitTestInTreeRootCoordinates(pointInFrameCoordinates);

    if (nullptr == visualHit)
    {
        // No visuals in this frame's visual tree were hit, so return nullptr.
        return nullptr;
    }

    auto it = std::find_if(m_peers.begin(), m_peers.end(), [&visualHit](std::shared_ptr<AutomationPeer> const& peer)
    {
        return peer->VisualNode() == visualHit;
    });

    if (m_peers.end() != it)
    {
        // A peer was found for the visual that was hit.
        // Check if the peer would like to forward the request to an external child (i.e. a different frame of content).
        // If not, the peer will return itself.
        return (*it)->ForwardFragmentFromPointInScreenCoordinatesRequest(x, y);
    }

    // No peer was found for the visual that was hit, but we know we are in the frame's bounds, so return the frame's fragment root.
    return m_root.as<::IRawElementProviderFragment>();
}

winrt::com_ptr<::IRawElementProviderFragment> AutomationTree::GetFragmentInFocusForAutomation(
    ::IUnknown const* const sender) const
{
    std::unique_lock lock{ m_mutex };

    if (sender != m_root.as<::IUnknown>().get())
    {
        return nullptr;
    }

    // Not doing anything currently.
    return nullptr;
}

winrt::com_ptr<::IRawElementProviderFragment> AutomationTree::GetParentFragmentForAutomation(
    ::IUnknown const* const sender) const
{
    std::unique_lock lock{ m_mutex };

    if (sender != m_root.as<::IUnknown>().get())
    {
        return nullptr;
    }

    return (nullptr != m_frameHostNoRef) ?
        m_frameHostNoRef->GetParentProviderForChildFrame(m_owningFrameNoRef) :
        m_owningFrameNoRef->GetIsland().ParentAutomationProvider().try_as<::IRawElementProviderFragment>();
}

winrt::com_ptr<::IRawElementProviderFragment> AutomationTree::GetNextSiblingFragmentForAutomation(
    ::IUnknown const* const sender) const
{
    std::unique_lock lock{ m_mutex };

    if (sender != m_root.as<::IUnknown>().get())
    {
        return nullptr;
    }

    return (nullptr != m_frameHostNoRef) ?
        m_frameHostNoRef->GetNextSiblingProviderForChildFrame(m_owningFrameNoRef) :
        m_owningFrameNoRef->GetIsland().NextSiblingAutomationProvider().try_as<::IRawElementProviderFragment>();
}

winrt::com_ptr<::IRawElementProviderFragment> AutomationTree::GetPreviousSiblingFragmentForAutomation(
    ::IUnknown const* const sender) const
{
    std::unique_lock lock{ m_mutex };

    if (sender != m_root.as<::IUnknown>().get())
    {
        return nullptr;
    }

    return (nullptr != m_frameHostNoRef) ?
        m_frameHostNoRef->GetPreviousSiblingProviderForChildFrame(m_owningFrameNoRef) :
        m_owningFrameNoRef->GetIsland().PreviousSiblingAutomationProvider().try_as<::IRawElementProviderFragment>();
}

winrt::com_ptr<::IRawElementProviderFragmentRoot> AutomationTree::GetFragmentRootForAutomation(
    ::IUnknown const* const sender) const
{
    std::unique_lock lock{ m_mutex };

    if (sender != m_root.as<::IUnknown>().get())
    {
        return nullptr;
    }

    return (nullptr != m_frameHostNoRef) ?
        m_frameHostNoRef->GetFragmentRootProviderForChildFrame(m_owningFrameNoRef) :
        m_owningFrameNoRef->GetIsland().FragmentRootAutomationProvider().try_as<::IRawElementProviderFragmentRoot>();
}

std::shared_ptr<AutomationPeer> AutomationTree::CreatePeer(
    _In_ std::shared_ptr<VisualTreeNode> const& visual,
    _In_ std::wstring_view const& name,
    _In_ long const& uiaControlTypeId)
{
    std::unique_lock lock{ m_mutex };
    return AutomationPeer::Create(m_owningFrameNoRef, visual, name, uiaControlTypeId);
}

void AutomationTree::AddPeer(
    _In_ std::shared_ptr<AutomationPeer> const& peer)
{
    std::unique_lock lock{ m_mutex };
    m_peers.push_back(peer);
}

void AutomationTree::SetFrameHost(
    _In_ IFrameHost const* const frameHost) noexcept
{
    std::unique_lock lock{ m_mutex };

    m_frameHostNoRef = frameHost;

    // If a frame host has been set, they are responsible for hooking up the automation tree.
    m_root->HostProvider(nullptr);
    m_contentIslandAutomationProviderRequestedRevoker.Revoke();
}

void AutomationTree::Initialize(
    _In_ IFrame const* const owningFrame,
    _In_ std::wstring_view const& name,
    _In_ long const& uiaControlTypeId)
{
    m_owningFrameNoRef = owningFrame;

    m_root = winrt::make_self<AutomationHelpers::AutomationFragmentRoot>();
    m_root->Name(name);
    m_root->IsContent(false);
    m_root->IsControl(false);
    m_root->UiaControlTypeId(uiaControlTypeId);
    m_fragmentCallbackRevoker = m_root->SetFragmentCallbackHandler(this);
    m_fragmentRootCallbackRevoker = m_root->SetFragmentRootCallbackHandler(this);
    m_externalParentCallbackRevoker = m_root->SetExternalParentCallbackHandler(this);

    // If this automation tree is not directly connected to an outer IFrameHost,
    // then we must connect into the system automation tree via the ContentIsland automation APIs.
    auto island = m_owningFrameNoRef->GetIsland();
    auto hostProviderAsIInspectable = island.GetAutomationHostProvider();
    if (nullptr != hostProviderAsIInspectable)
    {
        m_root->HostProvider(hostProviderAsIInspectable.as<::IRawElementProviderSimple>());
    }

    m_contentIslandAutomationProviderRequestedRevoker = { island, island.AutomationProviderRequested([weakThis=weak_from_this()](auto&&, auto&& args)
    {
        if (auto strongThis = weakThis.lock())
        {
            winrt::Windows::Foundation::IInspectable provider{ nullptr };
            winrt::copy_from_abi(provider, strongThis->m_root.get());
            args.AutomationProvider(provider);
            args.Handled(true);
        }
    }) };
}
