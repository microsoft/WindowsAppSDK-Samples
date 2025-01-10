// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once
#include "TextVisual.h"
#include "SettingCollection.h"

template<class T>
class CheckBox final : SettingChangedHandler
{
public:
    using ContainerVisual = typename CompositorTypes<T>::ContainerVisual;

    CheckBox(
        Output<T> const& output,
        std::wstring&& text,
        std::shared_ptr<SettingCollection> const& settings,
        SettingId id);

    // Gets the container visual, which is used for layout.
    auto& GetVisual() const noexcept
    {
        return m_containerVisual;
    }

    auto& GetText() const noexcept
    {
        return m_label.GetText();
    }

    winrt::Size Size() const
    {
        return m_size;
    }

    bool IsChecked() const noexcept;
    void IsChecked(bool value);

    void ToggleCheckState()
    {
        IsChecked(!IsChecked());
    }

private:
    void OnSettingChanged(SettingId id) override;

    ContainerVisual m_containerVisual;
    TextVisual<T> m_emptyCheckBox;
    TextVisual<T> m_filledCheckBox;
    TextVisual<T> m_label;
    winrt::Size m_size;
    SettingId m_settingId;
};

using LiftedCheckBox = CheckBox<winrt::Compositor>;
using SystemCheckBox = CheckBox<winrt::WUC::Compositor>;

// Inserts a CheckBox into a visual tree.
template <class T>
void InsertCheckBoxVisual(
    CheckBox<T> const& checkBox,
    std::shared_ptr<AutomationTree> const& automationTree,
    std::shared_ptr<AutomationPeer> const& parentPeer
    )
{
    auto visualNode = VisualTreeNode::Create(checkBox.GetVisual().as<IUnknown>());
    auto visualPeer = automationTree->CreatePeer(visualNode, checkBox.GetText(), UIA_CheckBoxControlTypeId);
    parentPeer->VisualNode()->AddChild(visualNode);
    parentPeer->Fragment()->AddChildToEnd(visualPeer->Fragment());
}
