// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.
#pragma once

#include <vector>
#include <string>
#include <winrt/Windows.Media.h>

namespace WindowsML
{
namespace Shared
{

    /// <summary>
    /// Image loading and preprocessing functionality
    /// </summary>
    class ImageProcessor
    {
    public:
        /// <summary>
        /// Load an image file asynchronously and return as VideoFrame
        /// </summary>
        static winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Media::VideoFrame> LoadImageFileAsync(winrt::hstring filePath);

        /// <summary>
        /// Convert VideoFrame to normalized tensor data for SqueezeNet
        /// </summary>
        static std::vector<float> BindVideoFrameAsTensor(const winrt::Windows::Media::VideoFrame& frame);

    private:
        static constexpr int IMAGE_SIZE = 224;
        static constexpr int CHANNELS = 3;

        // ImageNet normalization constants
        static constexpr float MEAN_R = 0.485f;
        static constexpr float MEAN_G = 0.456f;
        static constexpr float MEAN_B = 0.406f;
        static constexpr float STD_R = 0.229f;
        static constexpr float STD_G = 0.224f;
        static constexpr float STD_B = 0.225f;
    };

} // namespace Shared
} // namespace WindowsML
