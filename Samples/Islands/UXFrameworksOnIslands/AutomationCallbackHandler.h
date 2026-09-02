// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

#include <UIAutomation.h>
#include <wil/cppwinrt.h>
#include <winrt/Windows.Graphics.h>

namespace AutomationHelpers
{

enum class AutomationCallbackHandlerType : unsigned char
{
    None,
    Fragment,
    FragmentRoot,
    Invoke,
    ExternalChild,
    ExternalParent
};

struct IAutomationCallbackHandler
{
    virtual ~IAutomationCallbackHandler() noexcept = default;
};

struct IAutomationFragmentCallbackHandler : IAutomationCallbackHandler
{
    virtual winrt::Windows::Graphics::RectInt32 GetBoundingRectangleInScreenCoordinatesForAutomation(
        ::IUnknown const* const sender) const = 0;

    virtual void HandleSetFocusForAutomation(
        ::IUnknown const* const sender) = 0;
};

struct IAutomationFragmentRootCallbackHandler : IAutomationCallbackHandler
{
    virtual winrt::com_ptr<::IRawElementProviderFragment> GetFragmentFromPointInScreenCoordinatesForAutomation(
        double x,
        double y,
        ::IUnknown const* const sender) const = 0;

    virtual winrt::com_ptr<::IRawElementProviderFragment> GetFragmentInFocusForAutomation(
        ::IUnknown const* const sender) const = 0;
};

struct IAutomationInvokeCallbackHandler : IAutomationCallbackHandler
{
    virtual void HandleInvokeForAutomation(
        ::IUnknown const* const sender) = 0;
};

struct IAutomationExternalChildCallbackHandler : IAutomationCallbackHandler
{
    virtual winrt::com_ptr<::IRawElementProviderFragment> GetFirstChildFragmentForAutomation(
        ::IUnknown const* const sender) const = 0;

    virtual winrt::com_ptr<::IRawElementProviderFragment> GetLastChildFragmentForAutomation(
        ::IUnknown const* const sender) const = 0;
};

struct IAutomationExternalParentCallbackHandler : IAutomationCallbackHandler
{
    virtual winrt::com_ptr<::IRawElementProviderFragment> GetParentFragmentForAutomation(
        ::IUnknown const* const sender) const = 0;

    virtual winrt::com_ptr<::IRawElementProviderFragment> GetNextSiblingFragmentForAutomation(
        ::IUnknown const* const sender) const = 0;

    virtual winrt::com_ptr<::IRawElementProviderFragment> GetPreviousSiblingFragmentForAutomation(
        ::IUnknown const* const sender) const = 0;

    virtual winrt::com_ptr<::IRawElementProviderFragmentRoot> GetFragmentRootForAutomation(
        ::IUnknown const* const sender) const = 0;
};

struct AutomationCallbackHandlerEntry
{
    explicit AutomationCallbackHandlerEntry(
        AutomationCallbackHandlerType const& type,
        IAutomationCallbackHandler* const handler) :
        _type{ type }, _handler{ handler } {}

    bool Match(
        AutomationCallbackHandlerType const& type) const
    {
        return type == _type;
    }

    bool Match(
        IAutomationCallbackHandler const* const handler) const
    {
        return handler == _handler;
    }

    template <typename DerivedT>
    DerivedT* Get() const
    {
        static_assert(std::is_base_of_v<IAutomationCallbackHandler, DerivedT>);
        return static_cast<DerivedT*>(_handler);
    }

private:
    AutomationCallbackHandlerType _type{ AutomationCallbackHandlerType::None };
    IAutomationCallbackHandler* _handler{ nullptr };
};

}
