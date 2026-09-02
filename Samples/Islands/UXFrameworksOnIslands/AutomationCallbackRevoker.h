// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

#include "AutomationBase.h"

namespace AutomationHelpers
{

struct AutomationCallbackRevoker
{
    [[nodiscard]] static std::unique_ptr<AutomationCallbackRevoker> Create(
        winrt::weak_ref<AutomationBase> const& automationObject,
        IAutomationCallbackHandler* const handler) noexcept
    {
        try
        {
            auto newRevoker = std::make_unique<AutomationCallbackRevoker>();
            newRevoker->Initialize(automationObject, handler);
            return newRevoker;
        }
        catch (...) {}
        return nullptr;
    }

    explicit AutomationCallbackRevoker() noexcept = default;

    ~AutomationCallbackRevoker() noexcept
    {
        if (auto strongAutomationObject = _automationObject.get())
        {
            if (nullptr != _handler)
            {
                strongAutomationObject->RemoveHandler(_handler);
            }
        }
    }

    // Disable move and copy.
    explicit AutomationCallbackRevoker(AutomationCallbackRevoker const&) = delete;
    explicit AutomationCallbackRevoker(AutomationCallbackRevoker&&) = delete;
    AutomationCallbackRevoker& operator=(AutomationCallbackRevoker const&) = delete;
    AutomationCallbackRevoker& operator=(AutomationCallbackRevoker&&) = delete;

private:
    void Initialize(
        winrt::weak_ref<AutomationBase> const& automationObject,
        IAutomationCallbackHandler* const handler)
    {
        _automationObject = automationObject;
        _handler = handler;
    }

    winrt::weak_ref<AutomationBase> _automationObject{ nullptr };
    IAutomationCallbackHandler* _handler{ nullptr };
};

}
