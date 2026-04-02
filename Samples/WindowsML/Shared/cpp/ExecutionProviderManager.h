// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.
#pragma once

#include <winml/onnxruntime_cxx_api.h>
#include <winrt/Microsoft.Windows.AI.MachineLearning.h>
#include <optional>
#include <string>

namespace WindowsML
{
namespace Shared
{

    /// <summary>
    /// Execution Provider discovery and configuration functionality
    /// </summary>
    class ExecutionProviderManager
    {
    public:
        /// <summary>
        /// Initialize WinRT providers and register them with ONNX Runtime
        /// </summary>
        static void InitializeProviders(bool allowDownload = false);

        /// <summary>
        /// Configure a single, explicitly selected execution provider with optional device type.
        /// If the provider exposes multiple hardware device types, caller may specify one via --device_type.
        /// Returns true on success, false on validation/configuration failure (after printing guidance).
        /// This function demonstrates how to add a specific EP to the session options.
        /// </summary>
        static bool ConfigureSelectedExecutionProvider(Ort::SessionOptions& session_options,
                                                       Ort::Env& env,
                                                       const std::wstring& ep_name,
                                                       const std::optional<std::wstring>& device_type);

        /// <summary>
        /// Helper used by ArgumentParser help text: prints a table of currently discoverable EP devices.
        /// Safe to call even if providers have not yet been initialized; performs lightweight initialization.
        /// </summary>
        static void PrintExecutionProviderHelpTable();

    private:
        static void PrintExecutionProviderInfo(const std::vector<Ort::ConstEpDevice>& ep_devices);
    };

} // namespace Shared
} // namespace WindowsML
