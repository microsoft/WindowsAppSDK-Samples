// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

// Interface that represents a host that wraps a Win32 window and has specific focus-related
// operations.
struct IFocusHost
{
    virtual ~IFocusHost() = default;

    virtual void EnsureWin32Focus() const = 0;
};
