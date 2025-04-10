// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"
#include "CheckBox.h"
#include "TextRenderer.h"
#include "Output.h"

namespace
{
    constexpr wchar_t const* k_iconFont = L"Segoe MDL2 Assets";

    winrt::com_ptr<IDWriteTextLayout> const& GetEmptyCheckBoxTextLayout()
    {
        // L"\xE739" = CheckBox (empty box)
        static winrt::com_ptr<IDWriteTextLayout> textLayout = CreateTextLayout(L"\xE739", CreateTextFormat(k_iconFont).get());
        return textLayout;
    }

    winrt::com_ptr<IDWriteTextLayout> const& GetFilledCheckBoxTextLayout()
    {
        // L"\xE73A" = CheckBoxComposite (check box with check mark)
        static winrt::com_ptr<IDWriteTextLayout> textLayout = CreateTextLayout(L"\xE73A", CreateTextFormat(k_iconFont).get());
        return textLayout;
    }
}

template<class T>
CheckBox<T>::CheckBox(
        Output<T> const& output,
        std::wstring&& text,
        std::shared_ptr<SettingCollection> const& settings,
        SettingId id) :
    SettingChangedHandler(settings),
    m_containerVisual(output.GetCompositor().CreateContainerVisual()),
    m_emptyCheckBox(output, L"", GetEmptyCheckBoxTextLayout(), m_containerVisual),
    m_filledCheckBox(output, L"", GetFilledCheckBoxTextLayout(), m_containerVisual),
    m_label(output, std::move(text), nullptr, m_containerVisual),
    m_settingId(id)
{
    if (IsChecked())
    {
        m_emptyCheckBox.IsVisible(false);
    }
    else
    {
        m_filledCheckBox.IsVisible(false);
    }

    auto checkboxSize = m_emptyCheckBox.Size();
    auto labelSize = m_label.Size();
    m_label.SetTextOrigin({
        checkboxSize.Width + 4.0f,
        (checkboxSize.Height - labelSize.Height) / 2});

    m_size.Width = m_label.GetTextOrigin().x + m_label.Size().Width;
    m_size.Height = m_emptyCheckBox.Size().Height;

    m_containerVisual.Size(m_size);

    m_visualNode = VisualTreeNode::Create(m_containerVisual.as<::IUnknown>());
    m_visualNode->HitTestCallback([this](const HitTestContext& context) {
        if (context.IsRightClick())
        {
            return false;
        }

        ToggleCheckState();
        return true;
    });
}

template<class T>
bool CheckBox<T>::IsChecked() const noexcept
{
    return GetSetting(m_settingId);
}

template<class T>
void CheckBox<T>::IsChecked(bool value)
{
    GetSettings()->SetSetting(m_settingId, value);
}

template<class T>
void CheckBox<T>::OnSettingChanged(SettingId id)
{
    if (id == m_settingId)
    {
        bool isChecked = IsChecked();
        m_emptyCheckBox.IsVisible(!isChecked);
        m_filledCheckBox.IsVisible(isChecked);
    }
}

// Explicit template instantiation.
template class CheckBox<winrt::Compositor>;
template class CheckBox<winrt::WUC::Compositor>;
