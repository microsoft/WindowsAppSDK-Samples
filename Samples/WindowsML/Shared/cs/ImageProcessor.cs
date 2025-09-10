// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.

using Microsoft.ML.OnnxRuntime.Tensors;
using System.IO;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Graphics.Imaging;
using Windows.Media;
using Windows.Storage;
using Windows.Storage.Streams;

namespace WindowsML.Shared
{
    /// <summary>
    /// Image loading and preprocessing for ML inference
    /// </summary>
    public static class ImageProcessor
    {
        /// <summary>
        /// Load image file as VideoFrame
        /// </summary>
        public static async Task<VideoFrame> LoadImageFileAsync(string filePath)
        {
            try
            {
                StorageFile file = await StorageFile.GetFileFromPathAsync(filePath);
                IRandomAccessStream stream = await file.OpenAsync(FileAccessMode.Read);
                BitmapDecoder decoder = await BitmapDecoder.CreateAsync(stream);
                SoftwareBitmap softwareBitmap = await decoder.GetSoftwareBitmapAsync();
                VideoFrame inputImage = VideoFrame.CreateWithSoftwareBitmap(softwareBitmap);

                return inputImage;
            }
            catch (FileNotFoundException)
            {
                throw new FileNotFoundException($"Image file not found: {filePath}");
            }
            catch (Exception ex)
            {
                throw new Exception($"Error loading image: {ex.Message}", ex);
            }
        }

        /// <summary>
        /// Preprocess image for ML inference
        /// </summary>
        public static async Task<DenseTensor<float>> PreprocessImageAsync(VideoFrame videoFrame)
        {
            SoftwareBitmap softwareBitmap = videoFrame.SoftwareBitmap;
            const int targetWidth = 224;
            const int targetHeight = 224;

            float[] mean = [0.485f, 0.456f, 0.406f];
            float[] std = [0.229f, 0.224f, 0.225f];

            // Convert to BGRA8
            if (softwareBitmap.BitmapPixelFormat != BitmapPixelFormat.Bgra8 ||
                softwareBitmap.BitmapAlphaMode != BitmapAlphaMode.Premultiplied)
            {
                softwareBitmap = SoftwareBitmap.Convert(softwareBitmap, BitmapPixelFormat.Bgra8, BitmapAlphaMode.Premultiplied);
            }

            // Resize
            softwareBitmap = await ResizeSoftwareBitmapAsync(softwareBitmap, targetWidth, targetHeight);

            // Get pixel data
            uint bufferSize = (uint)(targetWidth * targetHeight * 4);
            Windows.Storage.Streams.Buffer buffer = new(bufferSize);
            softwareBitmap.CopyToBuffer(buffer);
            byte[] pixels = buffer.ToArray();

            // Output Tensor shape: [1, 3, 224, 224]
            DenseTensor<float> tensorData = new([1, 3, targetHeight, targetWidth]);

            for (int y = 0; y < targetHeight; y++)
            {
                for (int x = 0; x < targetWidth; x++)
                {
                    int pixelIndex = (y * targetWidth + x) * 4;
                    float r = pixels[pixelIndex + 2] / 255f;
                    float g = pixels[pixelIndex + 1] / 255f;
                    float b = pixels[pixelIndex + 0] / 255f;

                    // Normalize using mean/stddev
                    r = (r - mean[0]) / std[0];
                    g = (g - mean[1]) / std[1];
                    b = (b - mean[2]) / std[2];

                    int baseIndex = y * targetWidth + x;
                    tensorData[0, 0, y, x] = r; // R
                    tensorData[0, 1, y, x] = g; // G
                    tensorData[0, 2, y, x] = b; // B
                }
            }

            return tensorData;
        }

        /// <summary>
        /// Resize SoftwareBitmap to specified dimensions
        /// </summary>
        private static async Task<SoftwareBitmap> ResizeSoftwareBitmapAsync(SoftwareBitmap bitmap, int width, int height)
        {
            using InMemoryRandomAccessStream stream = new();
            BitmapEncoder encoder = await BitmapEncoder.CreateAsync(BitmapEncoder.PngEncoderId, stream);
            encoder.SetSoftwareBitmap(bitmap);
            encoder.IsThumbnailGenerated = false;
            await encoder.FlushAsync();
            stream.Seek(0);

            BitmapDecoder decoder = await BitmapDecoder.CreateAsync(stream);
            BitmapTransform transform = new() {
                ScaledWidth = (uint)width,
                ScaledHeight = (uint)height,
                InterpolationMode = BitmapInterpolationMode.Fant
            };
            SoftwareBitmap resized = await decoder.GetSoftwareBitmapAsync(
                BitmapPixelFormat.Bgra8,
                BitmapAlphaMode.Premultiplied,
                transform,
                ExifOrientationMode.IgnoreExifOrientation,
                ColorManagementMode.DoNotColorManage);

            stream.Dispose();
            return resized;
        }
    }
}
