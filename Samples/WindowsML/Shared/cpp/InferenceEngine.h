// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.
#pragma once

#include <winml/onnxruntime_cxx_api.h>
#include <vector>
#include <filesystem>
#include "ArgumentParser.h"

namespace WindowsML
{
namespace Shared
{

    /// <summary>
    /// ONNX Runtime inference execution functionality
    /// </summary>
    class InferenceEngine
    {
    public:
        /// <summary>
        /// Create and configure session options based on EP policy
        /// </summary>
        static Ort::SessionOptions CreateSessionOptions(const CommandLineOptions& options, Ort::Env& env);

        /// <summary>
        /// Determine which model to use (compiled vs original) and create session
        /// </summary>
        static std::filesystem::path DetermineModelPath(
            const CommandLineOptions& options,
            const std::filesystem::path& modelPath,
            const std::filesystem::path& compiledModelPath,
            Ort::SessionOptions& sessionOptions,
            Ort::Env& env);

        /// <summary>
        /// Create input tensor from preprocessed image data
        /// </summary>
        static Ort::Value CreateInputTensor(const std::vector<float>& inputTensorValues, const std::vector<int64_t>& inputShape);

        /// <summary>
        /// Run inference and return output tensor
        /// </summary>
        static std::vector<Ort::Value> RunInference(Ort::Session& session, const char* inputName, const char* outputName, Ort::Value& inputTensor);

        /// <summary>
        /// Extract results from output tensor
        /// </summary>
        static std::vector<float> ExtractResults(std::vector<Ort::Value>& outputTensors);

        /// <summary>
        /// Prepare input shape for inference (handle dynamic dimensions)
        /// </summary>
        static std::vector<int64_t> PrepareInputShape(const std::vector<int64_t>& originalShape);
    };

} // namespace Shared
} // namespace WindowsML
