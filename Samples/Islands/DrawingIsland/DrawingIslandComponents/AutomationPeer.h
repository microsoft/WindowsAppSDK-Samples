// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

namespace winrt::DrawingIslandComponents
{
    // For the DrawingIsland, each Visual corresponds to an Automation Fragment.
    // This structure encapsulates that relationship and enables us to quickly retrieve one from
    // the other.
    struct AutomationPeer
    {
        explicit AutomationPeer(
            winrt::Visual const& visual,
            winrt::com_ptr<AutomationFragment> const& automationProvider) :
            _visual{ visual },
            _automationProvider{ automationProvider }
        {

        }

        winrt::Visual const& GetVisual() const
        {
            return _visual;
        }

        winrt::com_ptr<AutomationFragment> const& GetAutomationProvider() const
        {
            return _automationProvider;
        }

        bool Match(winrt::Visual const& visual) const noexcept
        {
            return visual == _visual;
        }

        bool Match(::IUnknown const* const automationProviderAsIUnknown) const noexcept
        {
            return automationProviderAsIUnknown ==
                _automationProvider.try_as<::IUnknown>().get();
        }

    private:
        winrt::Visual _visual{ nullptr };
        winrt::com_ptr<AutomationFragment> _automationProvider{ nullptr };
    };
}
