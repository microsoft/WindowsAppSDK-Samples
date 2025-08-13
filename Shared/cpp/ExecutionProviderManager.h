// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.
#pragma once

#include <winml/onnxruntime_cxx_api.h>
#include <winrt/Microsoft.Windows.AI.MachineLearning.h>

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
        /// Configure execution providers by discovering and adding available EPs
        /// This function demonstrates how to:
        /// 1. Discover available execution provider devices
        /// 2. Configure them with provider-specific options
        /// 3. Add them to the session options
        /// </summary>
        static void ConfigureExecutionProviders(Ort::SessionOptions& session_options, Ort::Env& env);

    private:
        static void PrintExecutionProviderInfo(const std::vector<Ort::ConstEpDevice>& ep_devices);
    };

} // namespace Shared
} // namespace WindowsML
