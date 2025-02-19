// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

#include "FocusList.h"
#include "IFocusHost.h"

// A focus manager is responsible for determining what specific VisualTreeNode has focus as well as
// setting, clearing, and navigating focus.
// A focus manager can be responsible for multiple frames, each with their own FocusList that are
// assembled into a tree. This is what gets navigated when pressing tab.
// A focus manager will also send/respond to focus navigation requests for child and parent islands.
//
// A focus manager can be initialized in one of two ways:
// - With an InputFocusController, which is used for lifted islands
// - With an IFocusHost, which is used for the very root frame as a wrapper for a Win32 window.
class FocusManager
{
public:
    FocusManager(
        const std::shared_ptr<FocusList>& rootList);

    void InitializeWithFocusController(
        const winrt::InputFocusController& focusController);

    void InitializeWithFocusHost(
        IFocusHost* focusHost);

    void SetFocusFromClick(
        const std::shared_ptr<VisualTreeNode>& node,
        const std::shared_ptr<FocusList>& list);

    void ClearFocus();
    void RestoreFocus();

    void NavigateForward();
    void NavigateBackward();

    void SetFocusToFirst();
    void SetFocusToLast();

    // Called by focus lists

    void RegisterForDepartFocus(
        const winrt::InputFocusNavigationHost& navigationHost,
        const std::shared_ptr<FocusList>& list,
        size_t index);

private:
    const FocusListItem& CurrentItem() const { return (*m_focusedList)[m_focusedIndex]; }

    // The actual navigation implementation. This moves focus to a specific index in a specific list.
    // Note that navigation is always associated with a direction, so that the start or end of a
    // child list can be specified if needed.
    // Note also that 'currentIndex' does not need to be in bounds for the list.
    void NavigateWorker(
        const std::shared_ptr<FocusList>& currentList,
        size_t currentIndex,
        bool forward);

    // This helper does the walk through the tree of FocusLists to find a valid target.
    // If no target is found it will return nullptr.
    // 'currentIndex' does not need to be in bounds.
    static std::shared_ptr<FocusList> TryResolveNavigateTarget(
        const std::shared_ptr<FocusList>& currentList,
        size_t currentIndex,
        bool forward,
        size_t* outNewIndex);

    // Depart focus to the parent frame / focus host of the focus manager.
    void DepartImpl(bool forward);

    void EnsureWin32Focus();

    std::shared_ptr<FocusList> m_rootList{};
    std::shared_ptr<FocusList> m_focusedList{};
    size_t m_focusedIndex{ 0 };

    // Only one of these two will be valid depending on how the focus manager was initialized.
    winrt::InputFocusController m_focusController{ nullptr };
    IFocusHost* m_focusHost{ nullptr };
};