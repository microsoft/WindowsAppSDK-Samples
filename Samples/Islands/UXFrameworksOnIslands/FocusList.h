// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

struct VisualTreeNode;
class FocusManager;
class FocusList;

// FocusListItem is essentially a union that holds one of 4 values:
// - An empty placeholder
// - A VisualTreeNode
// - A FocusList
// - A winrt::InputFocusNavigationHost
struct FocusListItem
{
    std::variant<
        std::monostate,
        std::shared_ptr<VisualTreeNode>,
        std::shared_ptr<FocusList>,
        winrt::InputFocusNavigationHost> m_value;

    bool IsVisual() const { return std::holds_alternative<std::shared_ptr<VisualTreeNode>>(m_value); }
    bool IsChildList() const { return std::holds_alternative<std::shared_ptr<FocusList>>(m_value); }
    bool IsFocusNavigation() const { return std::holds_alternative<winrt::InputFocusNavigationHost>(m_value); }

    const std::shared_ptr<VisualTreeNode>& GetVisual() const { return std::get<std::shared_ptr<VisualTreeNode>>(m_value); }
    const std::shared_ptr<FocusList>& GetChildList() const { return std::get<std::shared_ptr<FocusList>>(m_value); }
    const winrt::InputFocusNavigationHost& GetFocusNavigation() const { return std::get<winrt::InputFocusNavigationHost>(m_value); }

    bool operator==(const std::shared_ptr<VisualTreeNode>& node) const
    {
        return IsVisual() && GetVisual() == node;
    }

    bool operator==(const std::shared_ptr<FocusList>& list) const
    {
        return IsChildList() && GetChildList() == list;
    }

    bool operator==(const winrt::InputFocusNavigationHost& navigationHost) const
    {
        return IsFocusNavigation() && GetFocusNavigation() == navigationHost;
    }
};

// A focus list is used to determine what order focus should navigate through when pressing tab.
// Since we have multiple frames that are connected together, focus operates on a tree of FocusLists,
// one for each frame. A child list is either added directly or indirectly through an
// InputNavigationHost which can send navigation requests to the connected island.
class FocusList : public std::enable_shared_from_this<FocusList>
{
public:
    static std::shared_ptr<FocusList> Create()
    {
        return std::make_shared<FocusList>();
    }

    void AddVisual(const std::shared_ptr<VisualTreeNode>& visualNode);
    void AddPlaceholder(); // To be filled with a FocusList or ChildSiteLink later

    // Fill previously set placeholder with a child FocusList
    void SetChildFocusList(const std::shared_ptr<FocusList>& childList, int index);

    // Fill previously set placeholder with a ChildSiteLink
    void SetChildSiteLink(const winrt::ChildSiteLink& link, int index);

    void SetManager(FocusManager* manager);
    FocusManager* GetManager() const { return m_managerWeak; }

    const FocusListItem& operator[](size_t index) const { return m_itemList[index]; }

    size_t Size() const { return m_itemList.size(); }
    std::shared_ptr<FocusList> GetParent() const { return m_parentList.lock(); }

    size_t IndexOf(const std::shared_ptr<VisualTreeNode>& node) const;
    size_t IndexOf(const std::shared_ptr<FocusList>& list) const;
    size_t IndexOf(const winrt::InputFocusNavigationHost& navigationHost) const;

private:
    std::vector<FocusListItem> m_itemList{};
    std::weak_ptr<FocusList> m_parentList{};
    FocusManager* m_managerWeak{ nullptr };
};
