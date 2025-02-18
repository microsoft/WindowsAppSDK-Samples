// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"
#include "FocusList.h"

#include "FocusManager.h"
#include "VisualTreeNode.h"

void FocusList::AddVisual(const std::shared_ptr<VisualTreeNode>& visualNode)
{
    m_itemList.emplace_back(FocusListItem{ visualNode });
    visualNode->OwningFocusList(weak_from_this());
}

void FocusList::AddPlaceholder()
{
    m_itemList.emplace_back(FocusListItem{});
}

void FocusList::SetChildFocusList(const std::shared_ptr<FocusList>& childList, int index)
{
    m_itemList[index].m_value = childList;

    childList->m_parentList = weak_from_this();

    if (m_managerWeak != nullptr)
    {
        childList->SetManager(m_managerWeak);
    }
}

void FocusList::SetChildSiteLink(const winrt::ChildSiteLink& link, int index)
{
    auto navigationHost = winrt::InputFocusNavigationHost::GetForSiteLink(link);

    m_itemList[index].m_value = navigationHost;

    if (m_managerWeak != nullptr)
    {
        m_managerWeak->RegisterForDepartFocus(navigationHost, shared_from_this(), index);
    }
}

void FocusList::SetManager(FocusManager* manager)
{
    m_managerWeak = manager;

    size_t index = 0;
    for (auto& item : m_itemList)
    {
        if (item.IsChildList())
        {
            item.GetChildList()->SetManager(manager);
        }
        else if (item.IsFocusNavigation())
        {
            manager->RegisterForDepartFocus(item.GetFocusNavigation(), shared_from_this(), index);
        }

        index++;
    }
}

size_t FocusList::IndexOf(const std::shared_ptr<VisualTreeNode>& node) const
{
    auto iter = std::find(m_itemList.begin(), m_itemList.end(), node);
    return std::distance(m_itemList.begin(), iter);
}

size_t FocusList::IndexOf(const std::shared_ptr<FocusList>& list) const
{
    auto iter = std::find(m_itemList.begin(), m_itemList.end(), list);
    return std::distance(m_itemList.begin(), iter);
}

size_t FocusList::IndexOf(const winrt::InputFocusNavigationHost& navigationHost) const
{
    auto iter = std::find(m_itemList.begin(), m_itemList.end(), navigationHost);
    return std::distance(m_itemList.begin(), iter);
}
