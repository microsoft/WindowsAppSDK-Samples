// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using Microsoft.Graphics.Imaging;
using Microsoft.UI.Xaml.Media.Imaging;
using System;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Graphics.Imaging;
using Windows.Storage.Streams;

namespace WindowsAISample.Util;

internal static class SoftwareBitmapExtensions
{
    public static async Task<SoftwareBitmapSource> ToSourceAsync(this SoftwareBitmap softwareBitmap)
    {
        var source = new SoftwareBitmapSource();

        if (softwareBitmap.BitmapPixelFormat != BitmapPixelFormat.Bgra8 || softwareBitmap.BitmapAlphaMode != BitmapAlphaMode.Premultiplied)
        {
            SoftwareBitmap convertedBitmap = SoftwareBitmap.Convert(softwareBitmap, BitmapPixelFormat.Bgra8, BitmapAlphaMode.Premultiplied);
            await source.SetBitmapAsync(convertedBitmap);
        }
        else
        {
            await source.SetBitmapAsync(softwareBitmap);
        }

        return source;
    }

    public static async Task<SoftwareBitmap> FilePathToSoftwareBitmapAsync(this string filePath)
    {
        using IRandomAccessStream stream = await StorageFileExtensions.CreateStreamAsync(filePath);
        // Create the decoder from the stream
        BitmapDecoder decoder = await BitmapDecoder.CreateAsync(stream);
        // Get the SoftwareBitmap representation of the file
        return await decoder.GetSoftwareBitmapAsync();
    }

    public static SoftwareBitmap CreateMaskBitmap(this SoftwareBitmap bitmap, Rect rect)
    {
        byte[] pixelData = new byte[bitmap.PixelWidth * bitmap.PixelHeight];

        // Fill the entire bitmap with black (0)
        for (int i = 0; i < pixelData.Length; i++)
        {
            pixelData[i] = 0; // Black
        }

        // Set the region bounded by the rectangle to white (255)
        for (int y = (int)rect.Y; y < (int)(rect.Y + rect.Height); y++)
        {
            for (int x = (int)rect.X; x < (int)(rect.X + rect.Width); x++)
            {
                pixelData[y * bitmap.PixelWidth + x] = 255; // White
            }
        }

        // Create a new SoftwareBitmap with Gray8 pixel format
        var maskBitmap = new SoftwareBitmap(BitmapPixelFormat.Gray8, bitmap.PixelWidth, bitmap.PixelHeight, BitmapAlphaMode.Ignore);

        maskBitmap.CopyFromBuffer(pixelData.AsBuffer());

        return maskBitmap;
    }

    public static SoftwareBitmap ApplyMask(this SoftwareBitmap inputBitmap, SoftwareBitmap grayMask)
    {
        if (inputBitmap.BitmapPixelFormat != BitmapPixelFormat.Bgra8 || grayMask.BitmapPixelFormat != BitmapPixelFormat.Gray8)
        {
            throw new Exception("Input bitmap must be Bgra8 and gray mask must be Gray8");
        }

        byte[] inputBuffer = new byte[4 * inputBitmap.PixelWidth * inputBitmap.PixelHeight];
        byte[] maskBuffer = new byte[grayMask.PixelWidth * grayMask.PixelHeight];
        inputBitmap.CopyToBuffer(inputBuffer.AsBuffer());
        grayMask.CopyToBuffer(maskBuffer.AsBuffer());

        for (int y = 0; y < inputBitmap.PixelHeight; y++)
        {
            for (int x = 0; x < inputBitmap.PixelWidth; x++)
            {
                int inputIndex = (y * inputBitmap.PixelWidth + x) * 4;
                int maskIndex = y * grayMask.PixelWidth + x;

                if (maskBuffer[maskIndex] == 0)
                {
                    inputBuffer[inputIndex + 3] = 0; // Set alpha to 0 for background
                }
            }
        }

        var segmentedBitmap = new SoftwareBitmap(BitmapPixelFormat.Bgra8, inputBitmap.PixelWidth, inputBitmap.PixelHeight);
        segmentedBitmap.CopyFromBuffer(inputBuffer.AsBuffer());
        return segmentedBitmap;
    }

    public static ImageBuffer ConvertToBgr8ImageBuffer(this SoftwareBitmap input)
    {
        var bgraBitmap = input;
        if (input.BitmapPixelFormat != BitmapPixelFormat.Bgra8)
        {
            bgraBitmap = SoftwareBitmap.Convert(input, BitmapPixelFormat.Bgra8, BitmapAlphaMode.Premultiplied);
        }

        int width = bgraBitmap.PixelWidth;
        int height = bgraBitmap.PixelHeight;

        byte[] bgraBuffer = new byte[width * height * 4];
        bgraBitmap.CopyToBuffer(bgraBuffer.AsBuffer());

        byte[] bgrBuffer = new byte[width * height * 3];
        for (int i = 0, j = 0; i < bgraBuffer.Length; i += 4, j += 3)
        {
            bgrBuffer[j] = bgraBuffer[i];
            bgrBuffer[j + 1] = bgraBuffer[i + 1];
            bgrBuffer[j + 2] = bgraBuffer[i + 2];
        }

        return ImageBuffer.CreateForBuffer(
            bgrBuffer.AsBuffer(),
            ImageBufferPixelFormat.Bgr8,
            width,
            height,
            width * 3);
    }

    public static SoftwareBitmap ConvertBgr8ImageBufferToBgra8SoftwareBitmap(this ImageBuffer bgrImageBuffer)
    {
        if (bgrImageBuffer.PixelFormat != ImageBufferPixelFormat.Bgr8)
        {
            throw new ArgumentException("Input ImageBuffer must be in Bgr8 format");
        }

        int width = bgrImageBuffer.PixelWidth;
        int height = bgrImageBuffer.PixelHeight;

        // Get BGR data from ImageBuffer
        byte[] bgrBuffer = new byte[width * height * 3];
        bgrImageBuffer.CopyToByteArray(bgrBuffer);

        // Create BGRA buffer (4 bytes per pixel)
        byte[] bgraBuffer = new byte[width * height * 4];

        for (int i = 0, j = 0; i < bgrBuffer.Length; i += 3, j += 4)
        {
            bgraBuffer[j] = bgrBuffer[i];           // B
            bgraBuffer[j + 1] = bgrBuffer[i + 1];   // G
            bgraBuffer[j + 2] = bgrBuffer[i + 2];   // R
            bgraBuffer[j + 3] = 255;                // A (full opacity)
        }

        // Create SoftwareBitmap and copy data
        var softwareBitmap = new SoftwareBitmap(
            BitmapPixelFormat.Bgra8,
            width,
            height,
            BitmapAlphaMode.Premultiplied);

        softwareBitmap.CopyFromBuffer(bgraBuffer.AsBuffer());

        return softwareBitmap;
    }
}
