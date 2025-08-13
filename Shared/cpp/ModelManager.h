// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.
#pragma once

#include <winml/onnxruntime_cxx_api.h>
#include <filesystem>
#include <vector>
#include <string>
#include <windows.h>

namespace WindowsML
{
namespace Shared
{

    /// <summary>
    /// ONNX model compilation and management functionality
    /// </summary>
    class ModelManager
    {
    public:
        /// <summary>
        /// Compile an ONNX model using the OrtCompileApi
        /// This function demonstrates how to:
        /// 1. Get the compile API
        /// 2. Create compilation options from session options
        /// 3. Configure input and output paths
        /// 4. Compile the model
        /// </summary>
        static OrtStatus* CompileModel(
            const OrtApi& ortApi,
            Ort::Env& env,
            OrtSessionOptions* sessionOptions,
            const std::filesystem::path& modelPath,
            const std::filesystem::path& compiledModelPath);

        /// <summary>
        /// Get the path to the executable directory
        /// </summary>
        static std::filesystem::path GetExecutablePath();

        /// <summary>
        /// Load labels from text file
        /// </summary>
        static std::vector<std::string> LoadLabels(const std::filesystem::path& labelsPath);

        /// <summary>
        /// Set default model paths based on executable location
        /// </summary>
        static void SetDefaultPaths(
            const std::filesystem::path& executableFolder,
            const std::wstring& modelPath,
            std::wstring& outputModelPath,
            std::wstring& outputImagePath);

    private:
        static std::filesystem::path GetModulePath(HMODULE module);
    };

} // namespace Shared
} // namespace WindowsML
