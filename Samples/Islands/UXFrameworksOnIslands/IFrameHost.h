// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

#include "IFrame.h"

struct IFrameHost
{
    virtual ~IFrameHost() = default;

    [[nodiscard]] virtual winrt::com_ptr<::IRawElementProviderFragmentRoot> GetFragmentRootProviderForChildFrame(_In_ IFrame const* const sender) const = 0;

    [[nodiscard]] virtual winrt::com_ptr<::IRawElementProviderFragment> GetNextSiblingProviderForChildFrame(_In_ IFrame const* const sender) const = 0;

    [[nodiscard]] virtual winrt::com_ptr<::IRawElementProviderFragment> GetParentProviderForChildFrame(_In_ IFrame const* const sender) const = 0;

    [[nodiscard]] virtual winrt::com_ptr<::IRawElementProviderFragment> GetPreviousSiblingProviderForChildFrame(_In_ IFrame const* const sender) const = 0;
};
