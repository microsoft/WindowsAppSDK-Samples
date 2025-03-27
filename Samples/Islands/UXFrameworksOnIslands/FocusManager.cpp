// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"
#include "FocusManager.h"

#include "FocusList.h"
#include "VisualTreeNode.h"

FocusManager::FocusManager(
    const std::shared_ptr<FocusList>& rootList) :
    m_rootList(rootList)
{
    m_rootList->SetManager(this);
}

void FocusManager::InitializeWithFocusController(
    const winrt::InputFocusController& focusController)
{
    m_focusController = focusController;

    // Listen for if our parent island is moving focus into this island.
    m_focusController.NavigateFocusRequested(
        [this](auto&& /*sender*/, const winrt::FocusNavigationRequestEventArgs& args) {
            if (args.Request().Reason() == winrt::FocusNavigationReason::First)
            {
                SetFocusToFirst();
            }
            else if (args.Request().Reason() == winrt::FocusNavigationReason::Last)
            {
                SetFocusToLast();
            }

            args.Result(winrt::FocusNavigationResult::Moved);
        });

    m_focusController.GotFocus(
        [this](auto&& /*sender*/, auto&& /*args*/)
    {
        RestoreFocus();
        });

    m_focusController.LostFocus(
        [this](auto&& /*sender*/, auto&& /*args*/)
    {
        ClearFocus();
        });
}

void FocusManager::InitializeWithFocusHost(
    IFocusHost* focusHost)
{
    m_focusHost = focusHost;
}

void FocusManager::SetFocusToVisual(
    const std::shared_ptr<VisualTreeNode>& node,
    const std::shared_ptr<FocusList>& list)
{
    ClearFocus();

    node->IsBorderVisible(true);

    m_focusedList = list;
    m_focusedIndex = list->IndexOf(node);
    EnsureWin32Focus();
}

void FocusManager::ClearFocus()
{
    if (m_focusedList != nullptr)
    {
        CurrentItem().GetVisual()->IsBorderVisible(false);
    }
}

void FocusManager::RestoreFocus()
{
    if (m_focusedList != nullptr)
    {
        CurrentItem().GetVisual()->IsBorderVisible(true);
    }
}

void FocusManager::NavigateForward()
{
    if (m_focusedList != nullptr)
    {
        NavigateWorker(m_focusedList, m_focusedIndex + 1, true /*forward*/);
    }
    else
    {
        SetFocusToFirst();
    }
}

void FocusManager::NavigateBackward()
{
    if (m_focusedList != nullptr)
    {
        NavigateWorker(m_focusedList, m_focusedIndex - 1, false /*forward*/);
    }
    else
    {
        SetFocusToLast();
    }
}

void FocusManager::SetFocusToFirst()
{
    NavigateWorker(m_rootList, 0, true /*forward*/);
}

void FocusManager::SetFocusToLast()
{
    size_t index = m_rootList->Size() - 1;
    NavigateWorker(m_rootList, index, false /*forward*/);
}

void FocusManager::RegisterForDepartFocus(
    const winrt::InputFocusNavigationHost& navigationHost,
    const std::shared_ptr<FocusList>& list,
    size_t index)
{
    std::weak_ptr<FocusList> weakList = list;
    navigationHost.DepartFocusRequested(
        [this, index, weakList](auto&& /*sender*/, const winrt::FocusNavigationRequestEventArgs& args) {
            auto list = weakList.lock();
            if (list == nullptr)
            {
                args.Result(winrt::FocusNavigationResult::NoFocusableElements);
                return;
            }

            // If the child hit its last element, and wants to depart, we need to navigate forward.
            bool forward = args.Request().Reason() == winrt::FocusNavigationReason::Last;

            size_t newIndex = forward ? index + 1 : index - 1;
            NavigateWorker(list, newIndex, forward);

            args.Result(winrt::FocusNavigationResult::Moved);
        });
}

void FocusManager::NavigateWorker(
    const std::shared_ptr<FocusList>& currentList,
    size_t currentIndex,
    bool forward)
{
    // Run additional logic to handle falling off the end of the list or going into a child list.
    size_t targetIndex = 0;
    std::shared_ptr<FocusList> targetList = TryResolveNavigateTarget(
        currentList,
        currentIndex,
        forward,
        &targetIndex);

    // Unset the focus border if we have any.
    ClearFocus();

    if (targetList == nullptr)
    {
        // No target - we need to depart focus out of this island.
        DepartImpl(forward);
        return;
    }

    auto& item = (*targetList)[targetIndex];

    if (item.IsVisual())
    {
        m_focusedList = targetList;
        m_focusedIndex = targetIndex;

        EnsureWin32Focus();
        item.GetVisual()->IsBorderVisible(true);
    }
    else if (item.IsFocusNavigation())
    {
        m_focusedList = nullptr;
        m_focusedIndex = 0;

        // Send request to push focus to child island.
        winrt::InputFocusNavigationHost focusNavigation = item.GetFocusNavigation();
        auto request = winrt::FocusNavigationRequest::Create(
            forward ? winrt::FocusNavigationReason::First : winrt::FocusNavigationReason::Last);
        auto result = focusNavigation.NavigateFocus(request);
        if (result == winrt::FocusNavigationResult::NoFocusableElements)
        {
            // Skip this one and try navigating again.
            targetIndex += forward ? 1 : -1;
            NavigateWorker(targetList, targetIndex, forward);
        }
    }
}

/*static*/
std::shared_ptr<FocusList> FocusManager::TryResolveNavigateTarget(
    const std::shared_ptr<FocusList>& currentList,
    size_t currentIndex,
    bool forward,
    size_t* outNewIndex)
{
    // A valid target for navigation is either a visual or a focus navigation host.
    // These operations need to be applied to find a valid target:
    // - If the index given is outside of the bounds ("falling off the end"), we need to navigate up
    //   to the parent list.
    // - If the index given is a child list, we need to navigate into that child list.
    // - If the index given is an empty placeholder, we need to skip it.
    //
    // If navigation ends up going off the end of the root list, then there is no valid target and
    // will return nullptr.

    std::shared_ptr<FocusList> nextList = currentList;
    size_t nextIndex = currentIndex;

    while (nextList != nullptr)
    {
        if (nextIndex >= 0 && nextIndex < nextList->Size())
        {
            // We're in bounds.
            auto& item = (*nextList)[nextIndex];
            if (item.IsChildList())
            {
                // Try the child list next.
                nextList = item.GetChildList();
                nextIndex = forward ? 0 : nextList->Size() - 1;
            }
            else if (item.IsVisual() || item.IsFocusNavigation())
            {
                // Found a valid target.
                break;
            }
            else
            {
                // Empty placeholder, skip it.
                nextIndex += forward ? 1 : -1;
            }
        }
        else
        {
            // Out of bounds.
            // Go up to the parent and increment.
            auto parent = nextList->GetParent();
            if (parent != nullptr)
            {
                nextIndex = parent->IndexOf(nextList);
                nextIndex += forward ? 1 : -1;
            }

            nextList = parent;
        }
    }

    *outNewIndex = nextIndex;
    return nextList;
}

void FocusManager::DepartImpl(
    bool forward)
{
    m_focusedList = nullptr;
    m_focusedIndex = 0;

    bool wrapAround = false;

    if (m_focusController != nullptr)
    {
        // If we're going forward then it means we've hit the Last element.
        auto reason = forward ? winrt::FocusNavigationReason::Last : winrt::FocusNavigationReason::First;
        auto request = winrt::FocusNavigationRequest::Create(reason);
        auto result = m_focusController.DepartFocus(request);
        if (result == winrt::FocusNavigationResult::NoFocusableElements)
        {
            wrapAround = true;
        }
    }
    else
    {
        // No focus controller means there is no parent frame (there's an IFocusHost instead), so
        // just wrap around.
        wrapAround = true;
    }

    if (wrapAround)
    {
        if (forward)
        {
            SetFocusToFirst();
        }
        else
        {
            SetFocusToLast();
        }
    }
}

void FocusManager::EnsureWin32Focus()
{
    if (m_focusController != nullptr)
    {
        if (!m_focusController.HasFocus())
        {
            m_focusController.TrySetFocus();
        }
    }
    else
    {
        m_focusHost->EnsureWin32Focus();
    }
}