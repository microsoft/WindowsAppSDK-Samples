// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml.Media.Imaging;
using Microsoft.Windows.Storage.Pickers;
using System;
using System.IO;
using System.Threading.Tasks;
using System.Windows.Input;
using Windows.Graphics.Imaging;
using Windows.Storage.Streams;
using WindowsAISample.Models.Contracts;
using WindowsAISample.Util;

namespace WindowsAISample.ViewModels;

internal abstract class InputImageViewModelBase<T> : CopilotModelBase<T>
    where T : IModelManager
{
    private const string DefaultFactoryImageFilePath = "Assets/horses.jpg";
    private const string DefaultFactoryMaskFilePath = "Assets/horsesMask.jpg";

    private readonly AsyncCommand<object, bool> _pickInputImageCommand;
    private readonly AsyncCommand<object, bool> _pickMaskImageCommand;
    private SoftwareBitmap? _input;
    private SoftwareBitmap? _mask;
    private SoftwareBitmapSource? _inputSource;
    private SoftwareBitmapSource? _maskSource;

    protected InputImageViewModelBase(T session) : base(session)
    {
        _ = Task.Run(async () =>
        {
            var inputSoftwareBitmap = await GetFactoryInputImagePath().FilePathToSoftwareBitmapAsync();
            var maskSoftwareBitmap = await GetFactoryMaskImagePath().FilePathToSoftwareBitmapAsync();
            var gray8MaskSoftwareBitmap = SoftwareBitmap.Convert(maskSoftwareBitmap, BitmapPixelFormat.Gray8);
            await DispatcherQueue.EnqueueAsync(async () =>
            {
                _input = inputSoftwareBitmap;
                _inputSource = await inputSoftwareBitmap.ToSourceAsync();
                OnPropertyChanged(nameof(InputSource));

                _mask = gray8MaskSoftwareBitmap;
                _maskSource = await gray8MaskSoftwareBitmap.ToSourceAsync();
                OnPropertyChanged(nameof(MaskSource));
            });
        });

        // Local helper to reduce duplication
        async Task<bool> PickImageAsync(Func<SoftwareBitmap, Task> setAction, bool convertToGray8 = false)
        {
            var picker = new FileOpenPicker(App.WindowId)
            {
                ViewMode = PickerViewMode.Thumbnail,
                SuggestedStartLocation = PickerLocationId.PicturesLibrary
            };
            picker.FileTypeFilter.Add(".jpg");
            picker.FileTypeFilter.Add(".jpeg");
            picker.FileTypeFilter.Add(".png");

            var file = await picker.PickSingleFileAsync();
            if (file == null || string.IsNullOrEmpty(file.Path))
                return false;

            using var fs = new FileStream(file.Path, FileMode.Open, FileAccess.Read, FileShare.Read, 4096, true);
            using IRandomAccessStream readStream = fs.AsRandomAccessStream();
            var decoder = await BitmapDecoder.CreateAsync(readStream);
            var softwareBitmap = await decoder.GetSoftwareBitmapAsync(BitmapPixelFormat.Bgra8, BitmapAlphaMode.Premultiplied);

            if (convertToGray8)
            {
                softwareBitmap = SoftwareBitmap.Convert(softwareBitmap, BitmapPixelFormat.Gray8);
            }

            await setAction(softwareBitmap);
            return true;
        }

        _pickInputImageCommand = new(async _ => await PickImageAsync(SetInputImageAsync), _ => true);
        _pickMaskImageCommand = new(async _ => await PickImageAsync(SetMaskImageAsync, convertToGray8: true), _ => true);
    }

    protected virtual string GetFactoryInputImagePath()
    {
        return DefaultFactoryImageFilePath;
    }

    protected virtual string GetFactoryMaskImagePath()
    {
        return DefaultFactoryMaskFilePath;
    }

    public ICommand PickInputImageCommand => _pickInputImageCommand;

    public ICommand PickMaskImageCommand => _pickMaskImageCommand;

    public SoftwareBitmapSource? InputSource
    {
        get
        {
            return _inputSource;
        }
        private set
        {
            _inputSource = value;
            OnPropertyChanged(nameof(InputSource));
        }
    }

    public SoftwareBitmap? Input => _input;

    public SoftwareBitmapSource? MaskSource
    {
        get
        {
            return _maskSource;
        }
        private set
        {
            _maskSource = value;
            OnPropertyChanged(nameof(MaskSource));
        }
    }

    public SoftwareBitmap? Mask => _mask;

    private async Task SetInputImageAsync(SoftwareBitmap softwareBitmap)
    {
        await DispatcherQueue.EnqueueAsync(async () =>
        {
            _input = softwareBitmap;
            InputSource = await softwareBitmap.ToSourceAsync();
        });
    }

    private async Task SetMaskImageAsync(SoftwareBitmap softwareBitmap)
    {
        await DispatcherQueue.EnqueueAsync(async () =>
        {
            _mask = softwareBitmap;
            MaskSource = await softwareBitmap.ToSourceAsync();
        });
    }
}
