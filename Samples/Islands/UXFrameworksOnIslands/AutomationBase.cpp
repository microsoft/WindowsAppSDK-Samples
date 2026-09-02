// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"
#include "AutomationBase.h"

namespace AutomationHelpers
{

void AutomationBase::RemoveHandler(
    IAutomationCallbackHandler const* const handler)
{
    std::unique_lock lock{ m_mutex };

    auto iterator = std::remove_if(
        m_handlers.begin(), m_handlers.end(), [handler](auto const& handlerEntry)
    {
        return handlerEntry.Match(handler);
    });

    m_handlers.erase(iterator, m_handlers.end());
}

void AutomationBase::AddHandler(
    AutomationCallbackHandlerType const& type,
    IAutomationCallbackHandler* const handler)
{
    // Remove any existing handler of the same type.
    auto iterator = std::remove_if(
        m_handlers.begin(), m_handlers.end(), [type](auto const& handlerEntry)
    {
        return handlerEntry.Match(type);
    });

    m_handlers.erase(iterator, m_handlers.end());

    if (nullptr != handler)
    {
        m_handlers.emplace_back(AutomationCallbackHandlerEntry{ type, handler });
    }
}

}
