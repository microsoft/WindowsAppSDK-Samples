// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"
#include "AutomationFragment.h"
#include <wil/resource.h>

using unique_safearray = wil::unique_any<SAFEARRAY*, decltype(&::SafeArrayDestroy), ::SafeArrayDestroy>;

namespace AutomationHelpers
{

std::unique_ptr<AutomationCallbackRevoker> AutomationFragment::SetFragmentCallbackHandler(
        IAutomationFragmentCallbackHandler* const handler)
{
    AddHandler(AutomationCallbackHandlerType::Fragment, handler);
    return AutomationCallbackRevoker::Create(GetWeak(), handler);
}

std::unique_ptr<AutomationCallbackRevoker> AutomationFragment::SetExternalChildCallbackHandler(
    IAutomationExternalChildCallbackHandler* const handler)
{
    AddHandler(AutomationCallbackHandlerType::ExternalChild, handler);
    return AutomationCallbackRevoker::Create(GetWeak(), handler);
}

std::unique_ptr<AutomationCallbackRevoker> AutomationFragment::SetExternalParentCallbackHandler(
    IAutomationExternalParentCallbackHandler* const handler)
{
    AddHandler(AutomationCallbackHandlerType::ExternalParent, handler);
    return AutomationCallbackRevoker::Create(GetWeak(), handler);
}

void AutomationFragment::AddChildToEnd(
    winrt::com_ptr<AutomationFragment> const& child)
{
    std::unique_lock lock{ m_mutex };

    if (nullptr == child)
    {
        // Nothing to do.
        return;
    }

    // The child should not already have a parent.
    winrt::check_bool(nullptr == child->Parent());

    // Set us up as the parent for the new child.
    child->Parent(GetWeak());

    // Set up the sibling relationships.
    if (!m_children.empty())
    {
        auto& previousSiblingForNewChild = m_children.back();
        previousSiblingForNewChild->NextSibling(child);
        child->PreviousSibling(previousSiblingForNewChild);
    }

    // Finally add the child.
    m_children.push_back(child);
}

void AutomationFragment::RemoveChild(
    winrt::com_ptr<AutomationFragment> const& child)
{
    std::unique_lock lock{ m_mutex };

    if (nullptr == child)
    {
        // Nothing to do.
        return;
    }

    auto iterator = std::find_if(
        m_children.begin(), m_children.end(), [&child](auto const& childEntry)
    {
        // See if we find a matching child entry in our children.
        return (childEntry.as<::IUnknown>().get() == child.as<::IUnknown>().get());
    });

    // We cannot remove a child that isn't ours.
    winrt::check_bool(m_children.end() != iterator);

    // Remove us from the parent relationship with the child.
    child->Parent(nullptr);

    // Reset the sibling relationships.
    auto previousSibling = child->PreviousSibling();
    auto nextSibling = child->NextSibling();
    if (nullptr != previousSibling)
    {
        previousSibling->NextSibling(nextSibling);
    }
    if (nullptr != nextSibling)
    {
        nextSibling->PreviousSibling(previousSibling);
    }
    child->PreviousSibling(nullptr);
    child->NextSibling(nullptr);

    // Finally, remove the child.
    m_children.erase(iterator);
}

void AutomationFragment::RemoveAllChildren()
{
    std::unique_lock lock{ m_mutex };

    for (auto& child : m_children)
    {
        // Disconnect the relationships from all our children.
        child->Parent(nullptr);
        child->PreviousSibling(nullptr);
        child->NextSibling(nullptr);
    }

    // Remove all the children.
    m_children.clear();
}

HRESULT __stdcall AutomationFragment::Navigate(
    _In_ NavigateDirection direction,
    _COM_Outptr_opt_result_maybenull_ ::IRawElementProviderFragment** fragment)
{
    std::unique_lock lock{ m_mutex };
    try
    {
        if (nullptr != fragment)
        {
            *fragment = nullptr;
            switch (direction)
            {
            case NavigateDirection_Parent:
            {
                if (auto handler = GetHandler<IAutomationExternalParentCallbackHandler>(AutomationCallbackHandlerType::ExternalParent))
                {
                    handler->GetParentFragmentForAutomation(GetIUnknown<AutomationFragment>()).copy_to(fragment);
                }
                else if (auto strongParent = LockWeak<AutomationFragment>(m_parent))
                {
                    strongParent.as<::IRawElementProviderFragment>().copy_to(fragment);
                }
                break;
            }
            case NavigateDirection_NextSibling:
            {
                if (auto handler = GetHandler<IAutomationExternalParentCallbackHandler>(AutomationCallbackHandlerType::ExternalParent))
                {
                    handler->GetNextSiblingFragmentForAutomation(GetIUnknown<AutomationFragment>()).copy_to(fragment);
                }
                else if (auto strongSibling = LockWeak<AutomationFragment>(m_nextSibling))
                {
                    strongSibling.as<::IRawElementProviderFragment>().copy_to(fragment);
                }
                break;
            }
            case NavigateDirection_PreviousSibling:
            {
                if (auto handler = GetHandler<IAutomationExternalParentCallbackHandler>(AutomationCallbackHandlerType::ExternalParent))
                {
                    handler->GetPreviousSiblingFragmentForAutomation(GetIUnknown<AutomationFragment>()).copy_to(fragment);
                }
                else if (auto strongSibling = LockWeak<AutomationFragment>(m_previousSibling))
                {
                    strongSibling.as<::IRawElementProviderFragment>().copy_to(fragment);
                }
                break;
            }
            case NavigateDirection_FirstChild:
            {
                if (auto handler = GetHandler<IAutomationExternalChildCallbackHandler>(AutomationCallbackHandlerType::ExternalChild))
                {
                    handler->GetFirstChildFragmentForAutomation(GetIUnknown<AutomationFragment>()).copy_to(fragment);
                }
                else if (!m_children.empty())
                {
                    m_children.front().as<::IRawElementProviderFragment>().copy_to(fragment);
                }
                break;
            }
            case NavigateDirection_LastChild:
            {
                if (auto handler = GetHandler<IAutomationExternalChildCallbackHandler>(AutomationCallbackHandlerType::ExternalChild))
                {
                    handler->GetLastChildFragmentForAutomation(GetIUnknown<AutomationFragment>()).copy_to(fragment);
                }
                else if (!m_children.empty())
                {
                    m_children.back().as<::IRawElementProviderFragment>().copy_to(fragment);
                }
                break;
            }
            }
        }
    }
    catch (...) { return UIA_E_ELEMENTNOTAVAILABLE; }
    return S_OK;
}

HRESULT __stdcall AutomationFragment::GetRuntimeId(
    _Outptr_opt_result_maybenull_ SAFEARRAY** runtimeId)
{
    std::unique_lock lock{ m_mutex };
    try
    {
        if (nullptr != runtimeId)
        {
            *runtimeId = nullptr;

            unsigned long arraySizeAsUnsignedLong = static_cast<unsigned long>(m_runtimeId.size());

            unique_safearray runtimeIdArray{ ::SafeArrayCreateVector(VT_I4, 0, arraySizeAsUnsignedLong) };
            SAFEARRAY* rawPointerToSafeArray = runtimeIdArray.get();
            winrt::check_pointer(rawPointerToSafeArray);

            for (long i = 0; i < static_cast<long>(arraySizeAsUnsignedLong); ++i)
            {
                winrt::check_hresult(::SafeArrayPutElement(rawPointerToSafeArray, &i, &(m_runtimeId[i])));
            }

            *runtimeId = runtimeIdArray.release();
        }
    }
    catch (...) { return UIA_E_ELEMENTNOTAVAILABLE; }
    return S_OK;
}

HRESULT __stdcall AutomationFragment::get_BoundingRectangle(
    _Out_ UiaRect* boundingRectangle)
{
    std::unique_lock lock{ m_mutex };
    try
    {
        if (nullptr != boundingRectangle)
        {
            *boundingRectangle = { 0, 0, 0, 0 };
            if (auto handler = GetHandler<IAutomationFragmentCallbackHandler>(AutomationCallbackHandlerType::Fragment))
            {
                auto screenRectangle =
                    handler->GetBoundingRectangleInScreenCoordinatesForAutomation(GetIUnknown<AutomationFragment>());

                boundingRectangle->left = screenRectangle.X;
                boundingRectangle->top = screenRectangle.Y;
                boundingRectangle->width = screenRectangle.Width;
                boundingRectangle->height = screenRectangle.Height;
            }
        }
    }
    catch (...) { return UIA_E_ELEMENTNOTAVAILABLE; }
    return S_OK;
}

HRESULT __stdcall AutomationFragment::GetEmbeddedFragmentRoots(
    _Outptr_opt_result_maybenull_ SAFEARRAY** embeddedFragmentRoots)
{
    std::unique_lock lock{ m_mutex };
    try
    {
        if (nullptr != embeddedFragmentRoots)
        {
            *embeddedFragmentRoots = nullptr;

            if (!m_embeddedFragments.empty())
            {
                unsigned long vectorSizeAsUnsignedLong = static_cast<unsigned long>(m_embeddedFragments.size());

                unique_safearray embeddedFragmentRootsArray{ ::SafeArrayCreateVector(VT_UNKNOWN, 0, vectorSizeAsUnsignedLong) };
                SAFEARRAY* rawPointerToSafeArray = embeddedFragmentRootsArray.get();
                winrt::check_pointer(rawPointerToSafeArray);

                for (long i = 0; i < static_cast<long>(vectorSizeAsUnsignedLong); ++i)
                {
                    winrt::check_hresult(::SafeArrayPutElement(rawPointerToSafeArray, &i, m_embeddedFragments.at(i).as<::IUnknown>().get()));
                }

                *embeddedFragmentRoots = embeddedFragmentRootsArray.release();
            }
        }
    }
    catch (...) { return UIA_E_ELEMENTNOTAVAILABLE; }
    return S_OK;
}

HRESULT __stdcall AutomationFragment::SetFocus()
{
    std::unique_lock lock{ m_mutex };
    try
    {
        if (auto handler = GetHandler<IAutomationFragmentCallbackHandler>(AutomationCallbackHandlerType::Fragment))
        {
            handler->HandleSetFocusForAutomation(GetIUnknown<AutomationFragment>());
        }
    }
    catch (...) { return UIA_E_ELEMENTNOTAVAILABLE; }
    return S_OK;
}

HRESULT __stdcall AutomationFragment::get_FragmentRoot(
    _COM_Outptr_opt_result_maybenull_ ::IRawElementProviderFragmentRoot** fragmentRoot)
{
    std::unique_lock lock{ m_mutex };
    try
    {
        if (nullptr != fragmentRoot)
        {
            *fragmentRoot = nullptr;

            winrt::com_ptr<::IRawElementProviderFragmentRoot> currentFragmentRootCandidate{ nullptr };
            if (auto handler = GetHandler<IAutomationExternalParentCallbackHandler>(AutomationCallbackHandlerType::ExternalParent))
            {
                currentFragmentRootCandidate = handler->GetFragmentRootForAutomation(GetIUnknown<AutomationFragment>());
            }
            else
            {
                // Walk up our fragment tree until we find our fragment root.
                auto fragmentRootCandidate = GetStrong<AutomationFragment>();
                bool currentCandidateIsThisObject = true;
                while (nullptr != fragmentRootCandidate && nullptr == fragmentRootCandidate.try_as<::IRawElementProviderFragmentRoot>())
                {
                    // Haven't found the fragment root yet, keep walking up our tree.
                    fragmentRootCandidate = currentCandidateIsThisObject ? LockWeak<AutomationFragment>(m_parent) : fragmentRootCandidate->Parent();
                    currentCandidateIsThisObject = false;
                }

                if (nullptr != fragmentRootCandidate)
                {
                    // Found the fragment root, return it.
                    currentFragmentRootCandidate = fragmentRootCandidate.as<::IRawElementProviderFragmentRoot>();
                }
            }

            // Check if the closest fragment root considers itself to be its own fragment root.
            // Sometimes we may find ourselves in an automation tree where the closest fragment root
            // is masquerading as a normal fragment with some ancestor as the true fragment root.
            auto strongThis = GetStrong<AutomationFragment>();
            if (nullptr != currentFragmentRootCandidate && strongThis.try_as<::IRawElementProviderFragmentRoot>().get() != currentFragmentRootCandidate.get())
            {
                winrt::com_ptr<::IRawElementProviderFragmentRoot> ancestorFragmentRoot{ currentFragmentRootCandidate };
                do
                {
                    currentFragmentRootCandidate = ancestorFragmentRoot;
                    auto currentFragmentRootAsFragment = currentFragmentRootCandidate.try_as<::IRawElementProviderFragment>();
                    if (nullptr != currentFragmentRootAsFragment)
                    {
                        winrt::check_hresult(currentFragmentRootAsFragment->get_FragmentRoot(ancestorFragmentRoot.put()));
                    }
                } while (nullptr != ancestorFragmentRoot && ancestorFragmentRoot.get() != currentFragmentRootCandidate.get());
            }

            currentFragmentRootCandidate.copy_to(fragmentRoot);
        }
    }
    catch (...) { return UIA_E_ELEMENTNOTAVAILABLE; }
    return S_OK;
}

}
