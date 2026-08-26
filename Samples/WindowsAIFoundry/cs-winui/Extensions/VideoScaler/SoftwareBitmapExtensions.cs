// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.Runtime.InteropServices.WindowsRuntime;
using Microsoft.Graphics.Imaging;
using Windows.Graphics.Imaging;

namespace WindowsAISample.Ext.VideoScaler;

internal static class SoftwareBitmapExtensions
{
    public static ImageBuffer ConvertToBgr8ImageBuffer(this SoftwareBitmap input)
    {
        var bgraBitmap = input;
        if (input.BitmapPixelFormat != BitmapPixelFormat.Bgra8)
        {
            bgraBitmap = SoftwareBitmap.Convert(
                input,
                BitmapPixelFormat.Bgra8,
                BitmapAlphaMode.Premultiplied);
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

    public static SoftwareBitmap ConvertBgr8ImageBufferToBgra8SoftwareBitmap(
        this ImageBuffer bgrImageBuffer)
    {
        if (bgrImageBuffer.PixelFormat != ImageBufferPixelFormat.Bgr8)
        {
            throw new ArgumentException(
                "Input ImageBuffer must be in Bgr8 format");
        }

        int width = bgrImageBuffer.PixelWidth;
        int height = bgrImageBuffer.PixelHeight;

        byte[] bgrBuffer = new byte[width * height * 3];
        bgrImageBuffer.CopyToByteArray(bgrBuffer);

        byte[] bgraBuffer = new byte[width * height * 4];
        for (int i = 0, j = 0; i < bgrBuffer.Length; i += 3, j += 4)
        {
            bgraBuffer[j] = bgrBuffer[i];
            bgraBuffer[j + 1] = bgrBuffer[i + 1];
            bgraBuffer[j + 2] = bgrBuffer[i + 2];
            bgraBuffer[j + 3] = 255;
        }

        var softwareBitmap = new SoftwareBitmap(
            BitmapPixelFormat.Bgra8,
            width,
            height,
            BitmapAlphaMode.Premultiplied);

        softwareBitmap.CopyFromBuffer(bgraBuffer.AsBuffer());
        return softwareBitmap;
    }
}
