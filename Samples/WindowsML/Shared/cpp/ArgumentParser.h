// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.
#pragma once

#include <optional>
#include <string>
#include <vector>
#include <winml/onnxruntime_cxx_api.h>

namespace WindowsML
{
namespace Shared
{

    /// <summary>
    /// Command-line options structure
    /// </summary>
    struct CommandLineOptions
    {
        bool compile_model = false;
        bool download_packages = false;
        std::optional<OrtExecutionProviderDevicePolicy> ep_policy;
        std::wstring image_path;
        std::wstring model_path;
        std::wstring output_path;
    };

    /// <summary>
    /// Argument parsing functionality
    /// </summary>
    class ArgumentParser
    {
    public:
        /// <summary>
        /// Parse command line arguments
        /// </summary>
        static bool ParseCommandLineArgs(int argc, wchar_t* argv[], CommandLineOptions& options);

        /// <summary>
        /// Convert execution provider policy to string for display
        /// </summary>
        static std::string ToString(OrtExecutionProviderDevicePolicy policy);

        /// <summary>
        /// Convert hardware device type to string for display
        /// </summary>
        static std::string ToString(OrtHardwareDeviceType policy);

    private:
        static void PrintUsage();
    };

} // namespace Shared
} // namespace WindowsML
