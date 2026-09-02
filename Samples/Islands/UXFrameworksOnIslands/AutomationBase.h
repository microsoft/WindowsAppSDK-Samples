// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

#include "AutomationCallbackHandler.h"

namespace AutomationHelpers
{

struct AutomationBase : winrt::implements<AutomationBase, winrt::Windows::Foundation::IInspectable>
{
    [[nodiscard]] winrt::weak_ref<AutomationBase> GetWeak() const noexcept
    {
        try
        {
            return const_cast<AutomationBase*>(this)->get_weak();
        }
        catch (...) {}
        return nullptr;
    }

    template <typename DerivedT>
    [[nodiscard]] inline static winrt::com_ptr<DerivedT> LockWeak(
        winrt::weak_ref<AutomationBase> const& weakRef) noexcept
    {
        static_assert(std::is_base_of_v<AutomationBase, DerivedT>);

        try
        {
            if (nullptr != weakRef)
            {
                winrt::com_ptr<AutomationBase> weakRefGet = weakRef.get();
                if (nullptr != weakRefGet)
                {
                    winrt::com_ptr<DerivedT> strongRef{ nullptr };
                    strongRef.copy_from(static_cast<DerivedT*>(weakRefGet.get()));
                    return strongRef;
                }
            }
        }
        catch (...) {}
        return nullptr;
    }

    template <typename DerivedT>
    [[nodiscard]] winrt::com_ptr<DerivedT> GetStrong() const noexcept
    {
        static_assert(std::is_base_of_v<AutomationBase, DerivedT>);

        try
        {
            return static_cast<DerivedT*>(const_cast<AutomationBase*>(this))->get_strong();
        }
        catch (...) {}
        return nullptr;
    }

    template <typename DerivedT>
    [[nodiscard]] IUnknown* GetIUnknown() const noexcept
    {
        static_assert(std::is_base_of_v<AutomationBase, DerivedT>);

        try
        {
            return GetStrong<DerivedT>().as<IUnknown>().get();
        }
        catch (...) {}
        return nullptr;
    }

    void RemoveHandler(
        IAutomationCallbackHandler const* const handler);

protected:
    void AddHandler(
        AutomationCallbackHandlerType const& type,
        IAutomationCallbackHandler* const handler);

    template <typename HandlerT>
    [[nodiscard]] HandlerT* GetHandler(
        AutomationCallbackHandlerType const& type) const
    {
        static_assert(std::is_base_of_v<IAutomationCallbackHandler, HandlerT>);

        auto iterator = std::find_if(
            m_handlers.cbegin(), m_handlers.cend(), [&type](auto const& handlerEntry)
        {
            return handlerEntry.Match(type);
        });

        return (m_handlers.cend() != iterator) ? iterator->Get<HandlerT>() : nullptr;
    }

    mutable std::mutex m_mutex{};

private:
    // Guard via m_mutex when accessing the following data members from all methods
    // that can be called from outside the AutomationHelpers namespace.
    std::vector<AutomationCallbackHandlerEntry> m_handlers{};
};

}
