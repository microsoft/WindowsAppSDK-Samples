// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using WindowsCopilotRuntimeSample.Models.Contracts;
using WindowsCopilotRuntimeSample.Util;
using Microsoft.UI.Xaml.Media.Imaging;
using System;
using System.Threading.Tasks;
using System.Windows.Input;
using Windows.Graphics.Imaging;
using Windows.Storage.Pickers;
using Windows.Storage.Streams;
using WinRT.Interop;

namespace WindowsCopilotRuntimeSample.ViewModels;

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
            await DispatcherQueue.EnqueueAsync(async () =>
            {
                _input = inputSoftwareBitmap;
                _inputSource = await inputSoftwareBitmap.ToSourceAsync();
                OnPropertyChanged(nameof(InputSource));

                _mask = maskSoftwareBitmap;
                _maskSource = await maskSoftwareBitmap.ToSourceAsync();
                OnPropertyChanged(nameof(MaskSource));
            });
        });

        _pickInputImageCommand = new(async _ =>
        {
            var picker = new FileOpenPicker();
            var window = App.Window;
            var hwnd = WindowNative.GetWindowHandle(window);
            InitializeWithWindow.Initialize(picker, hwnd);

            picker.ViewMode = PickerViewMode.Thumbnail;
            picker.SuggestedStartLocation = PickerLocationId.PicturesLibrary;
            picker.FileTypeFilter.Add(".jpg");
            picker.FileTypeFilter.Add(".jpeg");
            picker.FileTypeFilter.Add(".png");

            var file = await picker.PickSingleFileAsync();
            if (file != null)
            {
                using (IRandomAccessStream readStream = await file.OpenReadAsync())
                {
                    var decoder = await BitmapDecoder.CreateAsync(readStream);
                    var softwareBitmap = await decoder.GetSoftwareBitmapAsync(BitmapPixelFormat.Bgra8, BitmapAlphaMode.Premultiplied);
                    await SetInputImageAsync(softwareBitmap);
                }

                return true;
            }

            return false;

        },
        _ => true);

        _pickMaskImageCommand = new(async _ =>
        {
            var picker = new FileOpenPicker();
            var window = App.Window;
            var hwnd = WindowNative.GetWindowHandle(window);
            InitializeWithWindow.Initialize(picker, hwnd);

            picker.ViewMode = PickerViewMode.Thumbnail;
            picker.SuggestedStartLocation = PickerLocationId.PicturesLibrary;
            picker.FileTypeFilter.Add(".jpg");
            picker.FileTypeFilter.Add(".jpeg");
            picker.FileTypeFilter.Add(".png");

            var file = await picker.PickSingleFileAsync();
            if (file != null)
            {
                using (IRandomAccessStream readStream = await file.OpenReadAsync())
                {
                    var decoder = await BitmapDecoder.CreateAsync(readStream);
                    var softwareBitmap = await decoder.GetSoftwareBitmapAsync(BitmapPixelFormat.Bgra8, BitmapAlphaMode.Premultiplied);
                    await SetMaskImageAsync(softwareBitmap);
                }

                return true;
            }

            return false;

        },
        _ => true);
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
