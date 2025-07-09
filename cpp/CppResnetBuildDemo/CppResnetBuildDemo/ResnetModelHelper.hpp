// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.
#include <filesystem>
#include <Unknwn.h>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Graphics.Imaging.h>
#include <string>
#include <vector>

namespace ResnetModelHelper
{
    std::filesystem::path GetExecutablePath();

    winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Graphics::Imaging::SoftwareBitmap> LoadImageFileAsync(winrt::hstring filePath);

    std::vector<float> BindSoftwareBitmapAsTensor(const winrt::Windows::Graphics::Imaging::SoftwareBitmap& bitmap);

    std::vector<std::string> LoadLabels(const std::filesystem::path& labelsPath);

    std::vector<float> Softmax(const std::vector<float>& logits);

    std::vector<uint16_t> ConvertFloat32ToFloat16(const std::vector<float>& float32Data);

    std::vector<float> ConvertFloat16ToFloat32(const std::vector<uint16_t>& float16Data);

    void PrintResults(const std::vector<std::string>& labels, const std::vector<float>& results);
}
