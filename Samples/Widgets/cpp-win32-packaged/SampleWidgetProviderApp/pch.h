// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <stdint.h>
#include <combaseapi.h>

// In .exe local servers the class object must not contribute to the module ref count, and use
// winrt::no_module_lock, the other objects must and this is the hook into the C++ WinRT ref counting system
// that enables this.

void SignalLocalServerShutdown();

namespace winrt
{
    inline auto get_module_lock() noexcept
    {
        struct service_lock
        {
            uint32_t operator++() noexcept
            {
                return ::CoAddRefServerProcess();
            }

            uint32_t operator--() noexcept
            {
                const auto ref = ::CoReleaseServerProcess();

                if (ref == 0)
                {
                    SignalLocalServerShutdown();
                }
                return ref;
            }
        };

        return service_lock{};
    }
}

#define WINRT_CUSTOM_MODULE_LOCK

#include <wil/cppwinrt.h> // Already includes unknwn.h for COM support
#include <wil/resource.h>

#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.h>

#include <winrt/Microsoft.Windows.Widgets.Providers.h>

#include <ppltasks.h>

namespace winrt
{
    namespace Microsoft::Windows::Widgets {};
    using namespace Microsoft::Windows::Widgets;

    namespace Microsoft::Windows::Widgets::Providers {};
    using namespace Microsoft::Windows::Widgets::Providers;

    using namespace ::winrt::Windows::Storage;
    using namespace ::winrt::Windows::Foundation;
}
