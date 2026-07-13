//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the Microsoft Public License.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media.Imaging;
using Windows.Graphics.Imaging;
using Windows.Media.Capture.Frames;
using Windows.Media.MediaProperties;

namespace SDKTemplate
{
    internal sealed class FrameRenderer
    {
        private unsafe delegate void TransformScanline(int pixelWidth, byte* inputRowBytes, byte* outputRowBytes);

        private readonly Image _imageElement;
        private SoftwareBitmap? _backBuffer;
        private bool _taskRunning;

        public FrameRenderer(Image imageElement)
        {
            _imageElement = imageElement;
            _imageElement.Source = new SoftwareBitmapSource();
        }

        public void ProcessFrame(MediaFrameReference? frame)
        {
            if (frame == null)
            {
                return;
            }

            SoftwareBitmap? softwareBitmap = ConvertToDisplayableImage(frame.VideoMediaFrame);
            if (softwareBitmap == null)
            {
                return;
            }

            SoftwareBitmap? previousBitmap = Interlocked.Exchange(ref _backBuffer, softwareBitmap);
            previousBitmap?.Dispose();

            _imageElement.DispatcherQueue.TryEnqueue(() =>
            {
                if (_taskRunning)
                {
                    return;
                }

                _taskRunning = true;
                _ = DrainBackBufferAsync();
            });
        }

        public static string? GetSubtypeForFrameReader(MediaFrameSourceKind kind, MediaFrameFormat format)
        {
            string subtype = format.Subtype;
            switch (kind)
            {
                case MediaFrameSourceKind.Color:
                    return MediaEncodingSubtypes.Bgra8;

                case MediaFrameSourceKind.Depth:
                    return string.Equals(subtype, MediaEncodingSubtypes.D16, StringComparison.OrdinalIgnoreCase) ? subtype : null;

                case MediaFrameSourceKind.Infrared:
                    return string.Equals(subtype, MediaEncodingSubtypes.Nv12, StringComparison.OrdinalIgnoreCase) ||
                           string.Equals(subtype, MediaEncodingSubtypes.L8, StringComparison.OrdinalIgnoreCase) ||
                           string.Equals(subtype, MediaEncodingSubtypes.L16, StringComparison.OrdinalIgnoreCase)
                        ? subtype
                        : null;

                default:
                    return null;
            }
        }

        private async Task DrainBackBufferAsync()
        {
            while (true)
            {
                SoftwareBitmap? latestBitmap = Interlocked.Exchange(ref _backBuffer, null);
                if (latestBitmap == null)
                {
                    _taskRunning = false;
                    return;
                }

                if (_imageElement.Source is SoftwareBitmapSource imageSource)
                {
                    await imageSource.SetBitmapAsync(latestBitmap);
                }

                latestBitmap.Dispose();
            }
        }

        private static unsafe SoftwareBitmap? ConvertToDisplayableImage(VideoMediaFrame? inputFrame)
        {
            if (inputFrame?.SoftwareBitmap == null)
            {
                return null;
            }

            SoftwareBitmap inputBitmap = inputFrame.SoftwareBitmap;

            switch (inputFrame.FrameReference.SourceKind)
            {
                case MediaFrameSourceKind.Color:
                    if (inputBitmap.BitmapPixelFormat != BitmapPixelFormat.Bgra8)
                    {
                        return null;
                    }

                    return inputBitmap.BitmapAlphaMode == BitmapAlphaMode.Premultiplied
                        ? SoftwareBitmap.Copy(inputBitmap)
                        : SoftwareBitmap.Convert(inputBitmap, BitmapPixelFormat.Bgra8, BitmapAlphaMode.Premultiplied);

                case MediaFrameSourceKind.Depth:
                    if (inputBitmap.BitmapPixelFormat != BitmapPixelFormat.Gray16 || inputFrame.DepthMediaFrame == null)
                    {
                        return null;
                    }

                    float depthScale = (float)inputFrame.DepthMediaFrame.DepthFormat.DepthScaleInMeters;
                    uint minReliableDepth = inputFrame.DepthMediaFrame.MinReliableDepth;
                    uint maxReliableDepth = inputFrame.DepthMediaFrame.MaxReliableDepth;
                    return TransformBitmap(inputBitmap, (pixelWidth, inputRowBytes, outputRowBytes) =>
                        PseudoColorForDepth(pixelWidth, inputRowBytes, outputRowBytes, depthScale, minReliableDepth, maxReliableDepth));

                case MediaFrameSourceKind.Infrared:
                    return inputBitmap.BitmapPixelFormat switch
                    {
                        BitmapPixelFormat.Gray8 => TransformBitmap(inputBitmap, PseudoColorFor8BitInfrared),
                        BitmapPixelFormat.Gray16 => TransformBitmap(inputBitmap, PseudoColorFor16BitInfrared),
                        BitmapPixelFormat.Nv12 => SoftwareBitmap.Convert(inputBitmap, BitmapPixelFormat.Bgra8, BitmapAlphaMode.Premultiplied),
                        _ => null,
                    };

                default:
                    return null;
            }
        }

        private static unsafe SoftwareBitmap TransformBitmap(SoftwareBitmap inputBitmap, TransformScanline pixelTransformation)
        {
            var outputBitmap = new SoftwareBitmap(
                BitmapPixelFormat.Bgra8,
                inputBitmap.PixelWidth,
                inputBitmap.PixelHeight,
                BitmapAlphaMode.Premultiplied);

            using (BitmapBuffer input = inputBitmap.LockBuffer(BitmapBufferAccessMode.Read))
            using (BitmapBuffer output = outputBitmap.LockBuffer(BitmapBufferAccessMode.Write))
            using (var inputReference = input.CreateReference())
            using (var outputReference = output.CreateReference())
            {
                int inputStride = input.GetPlaneDescription(0).Stride;
                int outputStride = output.GetPlaneDescription(0).Stride;
                int pixelWidth = inputBitmap.PixelWidth;
                int pixelHeight = inputBitmap.PixelHeight;

                ((IMemoryBufferByteAccess)inputReference).GetBuffer(out byte* inputBytes, out _);
                ((IMemoryBufferByteAccess)outputReference).GetBuffer(out byte* outputBytes, out _);

                for (int y = 0; y < pixelHeight; y++)
                {
                    byte* inputRowBytes = inputBytes + y * inputStride;
                    byte* outputRowBytes = outputBytes + y * outputStride;
                    pixelTransformation(pixelWidth, inputRowBytes, outputRowBytes);
                }
            }

            return outputBitmap;
        }

        private static unsafe void PseudoColorForDepth(int pixelWidth, byte* inputRowBytes, byte* outputRowBytes, float depthScale, uint minReliableDepth, uint maxReliableDepth)
        {
            float minInMeters = minReliableDepth * depthScale;
            float maxInMeters = maxReliableDepth * depthScale;
            float oneMin = 1.0f / minInMeters;
            float range = 1.0f / maxInMeters - oneMin;

            ushort* inputRow = (ushort*)inputRowBytes;
            ColorBGRA* outputRow = (ColorBGRA*)outputRowBytes;
            for (int x = 0; x < pixelWidth; x++)
            {
                float depth = inputRow[x] * depthScale;
                outputRow[x] = depth == 0 ? default : PseudoColor(MathF.Pow((1.0f / depth - oneMin) / range, 2));
            }
        }

        private static unsafe void PseudoColorFor16BitInfrared(int pixelWidth, byte* inputRowBytes, byte* outputRowBytes)
        {
            ushort* inputRow = (ushort*)inputRowBytes;
            ColorBGRA* outputRow = (ColorBGRA*)outputRowBytes;
            for (int x = 0; x < pixelWidth; x++)
            {
                outputRow[x] = InfraredColor(inputRow[x] / (float)ushort.MaxValue);
            }
        }

        private static unsafe void PseudoColorFor8BitInfrared(int pixelWidth, byte* inputRowBytes, byte* outputRowBytes)
        {
            ColorBGRA* outputRow = (ColorBGRA*)outputRowBytes;
            for (int x = 0; x < pixelWidth; x++)
            {
                outputRow[x] = InfraredColor(inputRowBytes[x] / (float)byte.MaxValue);
            }
        }

        private static ColorBGRA PseudoColor(float value) => _colorLookupTable.GetValue(value);

        private static ColorBGRA InfraredColor(float value) => _infraredLookupTable.GetValue(value);

        private static ColorBGRA GeneratePseudoColorLookupTable(int index, int size)
            => ColorRampInterpolation(index / (float)size);

        private static ColorBGRA GenerateInfraredRampLookupTable(int index, int size)
            => ColorRampInterpolation(MathF.Pow(1 - index / (float)size, 12));

        private static ColorBGRA ColorRampInterpolation(float value)
        {
            int rampSteps = _colorRamp.Length - 1;
            float scaled = value * rampSteps;
            int integer = (int)scaled;
            int index = Math.Clamp(integer, 0, rampSteps - 1);

            ColorBGRA previous = _colorRamp[index];
            ColorBGRA next = _colorRamp[index + 1];
            uint alpha = (uint)((scaled - integer) * 255);
            uint beta = 255 - alpha;

            return new ColorBGRA
            {
                B = (byte)((previous.A * beta + next.A * alpha) / 255),
                G = (byte)((previous.R * beta + next.R * alpha) / 255),
                R = (byte)((previous.G * beta + next.G * alpha) / 255),
                A = (byte)((previous.B * beta + next.B * alpha) / 255),
            };
        }

        private static readonly ColorBGRA[] _colorRamp =
        {
            new() { B = 0xFF, G = 0x7F, R = 0x00, A = 0x00 },
            new() { B = 0xFF, G = 0xFF, R = 0x00, A = 0x00 },
            new() { B = 0xFF, G = 0xFF, R = 0x7F, A = 0x00 },
            new() { B = 0xFF, G = 0xFF, R = 0xFF, A = 0x00 },
            new() { B = 0xFF, G = 0x7F, R = 0xFF, A = 0x7F },
            new() { B = 0xFF, G = 0x00, R = 0xFF, A = 0xFF },
            new() { B = 0xFF, G = 0x00, R = 0x7F, A = 0xFF },
            new() { B = 0xFF, G = 0x00, R = 0x00, A = 0xFF },
            new() { B = 0xFF, G = 0x00, R = 0x00, A = 0x7F },
        };

        private static readonly LookupTable<ColorBGRA> _colorLookupTable = new(GeneratePseudoColorLookupTable);
        private static readonly LookupTable<ColorBGRA> _infraredLookupTable = new(GenerateInfraredRampLookupTable);

        [StructLayout(LayoutKind.Sequential)]
        private struct ColorBGRA
        {
            public byte B;
            public byte G;
            public byte R;
            public byte A;
        }

        private sealed class LookupTable<T>
        {
            private const int LookupTableSize = 1024;
            private readonly T[] _lookupTable = new T[LookupTableSize];

            public LookupTable(Func<int, int, T> generator)
            {
                for (int i = 0; i < LookupTableSize; i++)
                {
                    _lookupTable[i] = generator(i, LookupTableSize);
                }
            }

            public T GetValue(float value)
            {
                int index = Math.Clamp((int)(value * LookupTableSize), 0, LookupTableSize - 1);
                return _lookupTable[index];
            }
        }
    }
}
