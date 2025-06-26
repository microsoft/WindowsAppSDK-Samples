// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.
#include "ResnetModelHelper.hpp"

// clang-format off
#include <winrt/base.h>
#include <wil/win32_helpers.h>
// clang-format on

#include <filesystem>
#include <fstream>
#include <iostream>
#include <MemoryBuffer.h>
#include <string>
#include <vector>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Imaging.h>
#include <winrt/Windows.Media.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>

using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Graphics::Imaging;
using namespace winrt::Windows::Media;
using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::Storage;
using winrt::Windows::Foundation::IAsyncOperation;
using winrt::Windows::Foundation::IMemoryBufferReference;

namespace ResnetModelHelper
{
    std::filesystem::path GetExecutablePath()
    {
        auto filePath = wil::GetModuleFileNameW<wil::unique_cotaskmem_string>();
        std::filesystem::path fil{filePath.get()};
        return fil;
    }

    IAsyncOperation<SoftwareBitmap> LoadImageFileAsync(winrt::hstring filePath)
    {
        StorageFile file = co_await StorageFile::GetFileFromPathAsync(filePath);
        auto stream = co_await file.OpenAsync(FileAccessMode::Read);
        BitmapDecoder decoder = co_await BitmapDecoder::CreateAsync(stream);
        SoftwareBitmap softwareBitmap = co_await decoder.GetSoftwareBitmapAsync();

        co_return softwareBitmap;
    }

    std::vector<float> BindSoftwareBitmapAsTensor(SoftwareBitmap const& bitmap)
    {
        SoftwareBitmap bitmapBgra8 = SoftwareBitmap::Convert(bitmap, BitmapPixelFormat::Bgra8, BitmapAlphaMode::Ignore);

        winrt::Windows::Storage::Streams::InMemoryRandomAccessStream stream = InMemoryRandomAccessStream();

        BitmapEncoder encoder = BitmapEncoder::CreateAsync(BitmapEncoder::BmpEncoderId(), stream).get();
        encoder.SetSoftwareBitmap(bitmapBgra8);
        encoder.BitmapTransform().ScaledWidth(224);
        encoder.BitmapTransform().ScaledHeight(224);
        encoder.BitmapTransform().InterpolationMode(BitmapInterpolationMode::Fant);
        encoder.FlushAsync().get();

        stream.Seek(0);
        BitmapDecoder decoder = BitmapDecoder::CreateAsync(stream).get();
        SoftwareBitmap resizedBitmap =
            decoder.GetSoftwareBitmapAsync(BitmapPixelFormat::Bgra8, BitmapAlphaMode::Ignore).get();

        BitmapBuffer bitmapBuffer = resizedBitmap.LockBuffer(BitmapBufferAccessMode::Read);

        IMemoryBufferReference reference = bitmapBuffer.CreateReference();

        auto spByteAccess = reference.as<::Windows::Foundation::IMemoryBufferByteAccess>();
        uint8_t* pixelData = nullptr;
        uint32_t pixelDataCapacity = 0;

        spByteAccess->GetBuffer(&pixelData, &pixelDataCapacity);

        const int64_t channels = 3; // RGB
        const int64_t height = 224;
        const int64_t width = 224;

        std::vector<int64_t> tensorShape = {1, channels, height, width};
        std::vector<float> tensorData(channels * height * width);

        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                int idx = (y * width + x) * 4; // BGRA stride
                int offset = y * width + x;

                // Normalize using mean=[0.485, 0.456, 0.406] and std=[0.229, 0.224, 0.225]
                float r = static_cast<float>(pixelData[idx + 2]) / 255.0f;
                float g = static_cast<float>(pixelData[idx + 1]) / 255.0f;
                float b = static_cast<float>(pixelData[idx + 0]) / 255.0f;

                tensorData[0 * height * width + offset] = (r - 0.485f) / 0.229f;
                tensorData[1 * height * width + offset] = (g - 0.456f) / 0.224f;
                tensorData[2 * height * width + offset] = (b - 0.406f) / 0.225f;
            }
        }
        return tensorData;
    }

    std::vector<std::string> LoadLabels(const std::filesystem::path& labelsPath)
    {
        std::ifstream labelFile{labelsPath, std::ifstream::in};
        if (labelFile.fail())
        {
            throw std::runtime_error("Unable to load file.");
        }

        std::vector<std::string> labels;
        for (std::string line; std::getline(labelFile, line);)
        {
            labels.push_back(line);
        }

        return labels;
    }

    std::vector<float> Softmax(const std::vector<float>& logits)
    {
        std::vector<float> exps(logits.size());
        float maxLogit = *std::max_element(logits.begin(), logits.end());

        float sum = 0.0f;
        for (size_t i = 0; i < logits.size(); ++i)
        {
            exps[i] = std::exp(logits[i] - maxLogit); // stability trick
            sum += exps[i];
        }

        for (float& val : exps)
        {
            val /= sum;
        }

        return exps;
    }

    /* Simple IEEE 754 half-precision (float16) conversion utility in C/C++. Can be replaced with half.hpp or other FP16
     * libraries if available. */
    uint16_t Float32ToFloat16(float value)
    {
        uint32_t asInt;
        memcpy(&asInt, &value, sizeof(value));

        uint32_t sign = (asInt >> 16) & 0x8000;
        uint32_t exponent = ((asInt >> 23) & 0xFF) - 127 + 15;
        uint32_t mantissa = asInt & 0x007FFFFF;

        if (exponent <= 0)
        {
            return static_cast<uint16_t>(sign);
        }
        else if (exponent >= 31)
        {
            return static_cast<uint16_t>(sign | 0x7C00); // INF
        }

        uint16_t half = static_cast<uint16_t>(sign | (exponent << 10) | (mantissa >> 13));
        return half;
    }

    std::vector<uint16_t> ConvertFloat32ToFloat16(const std::vector<float>& float32Data)
    {
        std::vector<uint16_t> float16Data(float32Data.size());
        for (size_t i = 0; i < float32Data.size(); ++i)
        {
            float16Data[i] = Float32ToFloat16(float32Data[i]);
        }
        return float16Data;
    }

    float Float16ToFloat32(uint16_t value)
    {
        uint32_t sign = (value >> 15) & 0x1;
        uint32_t exponent = (value >> 10) & 0x1F;
        uint32_t mantissa = value & 0x3FF;

        if (exponent == 0)
        {
            return static_cast<float>((sign ? -1 : 1) * 0.0f);
        }
        else if (exponent == 31)
        {
            return (sign ? -1 : 1) * (mantissa == 0 ? std::numeric_limits<float>::infinity() : std::nanf(""));
        }

        exponent = exponent + (127 - 15); // Bias correction
        mantissa = mantissa << 13;        // Shift mantissa to float position

        uint32_t result = (sign << 31) | (exponent << 23) | mantissa;
        return *reinterpret_cast<float*>(&result);
    }

    std::vector<float> ConvertFloat16ToFloat32(const std::vector<uint16_t>& float16Data)
    {
        std::vector<float> float32Data(float16Data.size());
        for (size_t i = 0; i < float16Data.size(); ++i)
        {
            float32Data[i] = Float16ToFloat32(float16Data[i]);
        }
        return float32Data;
    }

    void PrintResults(const std::vector<std::string>& labels, const std::vector<float>& results) {
        // Apply softmax to the results  
        float maxLogit = *std::max_element(results.begin(), results.end());
        std::vector<float> expScores;
        float sumExp = 0.0f;

        for (float r : results) {
            float expScore = std::exp(r - maxLogit);
            expScores.push_back(expScore);
            sumExp += expScore;
        }

        std::vector<float> softmaxResults;
        for (float e : expScores) {
            softmaxResults.push_back(e / sumExp);
        }

        // Get top 5 results  
        std::vector<std::pair<int, float>> indexedResults;
        for (size_t i = 0; i < softmaxResults.size(); ++i) {
            indexedResults.emplace_back(static_cast<int>(i), softmaxResults[i]);
        }

        std::sort(indexedResults.begin(), indexedResults.end(), [](const auto& a, const auto& b) {
            return a.second > b.second;
            });

        indexedResults.resize(std::min<size_t>(5, indexedResults.size()));

        // Display results  
        std::cout << "Top Predictions:\n";
        std::cout << "-------------------------------------------\n";
        std::cout << std::left << std::setw(32) << "Label" << std::right << std::setw(10) << "Confidence\n";
        std::cout << "-------------------------------------------\n";

        for (const auto& result : indexedResults) {
            std::cout << std::left << std::setw(32) << labels[result.first]
                << std::right << std::setw(10) << std::fixed << std::setprecision(2) << (result.second * 100) << "%\n";
        }

        std::cout << "-------------------------------------------\n";
    }

}
