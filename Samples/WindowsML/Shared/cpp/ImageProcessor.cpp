// Copyright (c) Microsoft Corporation. All rights r        auto spByteAccess = reference.as<::Windows::Foundation::IMemoryBufferByteAccess>();served.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.
#include "ImageProcessor.h"
#include <MemoryBuffer.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Graphics.Imaging.h>
#include <winrt/Windows.Foundation.h>

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Graphics::Imaging;
using namespace winrt::Windows::Media;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::Storage::Streams;

namespace WindowsML
{
namespace Shared
{

    IAsyncOperation<VideoFrame> ImageProcessor::LoadImageFileAsync(winrt::hstring filePath)
    {
        StorageFile file = co_await StorageFile::GetFileFromPathAsync(filePath);
        auto stream = co_await file.OpenAsync(FileAccessMode::Read);
        BitmapDecoder decoder = co_await BitmapDecoder::CreateAsync(stream);
        SoftwareBitmap softwareBitmap = co_await decoder.GetSoftwareBitmapAsync();
        VideoFrame inputImage = VideoFrame::CreateWithSoftwareBitmap(softwareBitmap);

        co_return inputImage;
    }

    std::vector<float> ImageProcessor::BindVideoFrameAsTensor(const VideoFrame& frame)
    {
        SoftwareBitmap bitmap = frame.SoftwareBitmap();
        SoftwareBitmap bitmapBgra8 = SoftwareBitmap::Convert(bitmap, BitmapPixelFormat::Bgra8, BitmapAlphaMode::Ignore);

        InMemoryRandomAccessStream stream = InMemoryRandomAccessStream();

        BitmapEncoder encoder = BitmapEncoder::CreateAsync(BitmapEncoder::BmpEncoderId(), stream).get();
        encoder.SetSoftwareBitmap(bitmapBgra8);
        encoder.BitmapTransform().ScaledWidth(IMAGE_SIZE);
        encoder.BitmapTransform().ScaledHeight(IMAGE_SIZE);
        encoder.BitmapTransform().InterpolationMode(BitmapInterpolationMode::Fant);
        encoder.FlushAsync().get();

        stream.Seek(0);
        BitmapDecoder decoder = BitmapDecoder::CreateAsync(stream).get();
        SoftwareBitmap resizedBitmap = decoder.GetSoftwareBitmapAsync(BitmapPixelFormat::Bgra8, BitmapAlphaMode::Ignore).get();

        BitmapBuffer bitmapBuffer = resizedBitmap.LockBuffer(BitmapBufferAccessMode::Read);

        IMemoryBufferReference reference = bitmapBuffer.CreateReference();

        auto spByteAccess = reference.as<::Windows::Foundation::IMemoryBufferByteAccess>();
        uint8_t* pixelData = nullptr;
        uint32_t pixelDataCapacity = 0;

        spByteAccess->GetBuffer(&pixelData, &pixelDataCapacity);

        std::vector<float> tensorData(CHANNELS * IMAGE_SIZE * IMAGE_SIZE);

        for (int y = 0; y < IMAGE_SIZE; ++y)
        {
            for (int x = 0; x < IMAGE_SIZE; ++x)
            {
                int idx = (y * IMAGE_SIZE + x) * 4; // BGRA stride
                int offset = y * IMAGE_SIZE + x;

                // The required input size is (3x224x224),
                // normalized using mean=[0.485, 0.456, 0.406]
                // and std=[0.229, 0.224, 0.225]
                float r = static_cast<float>(pixelData[idx + 2]) / 255.0f;
                float g = static_cast<float>(pixelData[idx + 1]) / 255.0f;
                float b = static_cast<float>(pixelData[idx + 0]) / 255.0f;

                tensorData[0 * IMAGE_SIZE * IMAGE_SIZE + offset] = (r - MEAN_R) / STD_R;
                tensorData[1 * IMAGE_SIZE * IMAGE_SIZE + offset] = (g - MEAN_G) / STD_G;
                tensorData[2 * IMAGE_SIZE * IMAGE_SIZE + offset] = (b - MEAN_B) / STD_B;
            }
        }

        return tensorData;
    }

} // namespace Shared
} // namespace WindowsML
