// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

namespace winrt::DrawingIslandComponents
{
    struct __declspec(novtable) IAutomationCallbackHandler
    {
        virtual ~IAutomationCallbackHandler() noexcept = default;

        virtual winrt::Windows::Graphics::RectInt32 GetScreenBoundsForAutomationFragment(
            _In_::IUnknown const* const sender) const = 0;

        virtual winrt::com_ptr<IRawElementProviderFragment> GetFragmentFromPoint(
            _In_ double x,
            _In_ double y) const = 0;

        virtual winrt::com_ptr<IRawElementProviderFragment> GetFragmentInFocus() const = 0;
    };
}
