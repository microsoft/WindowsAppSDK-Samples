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

    private readonly AsyncCommand<object, bool> _pickInputImageCommand;
    private SoftwareBitmap? _input;
    private SoftwareBitmapSource? _inputSource;

    protected InputImageViewModelBase(T session) : base(session)
    {
        _ = Task.Run(async () =>
        {
            var softwareBitmap = await GetFactoryInputImagePath().FilePathToSoftwareBitmapAsync();
            await DispatcherQueue.EnqueueAsync(async () =>
            {
                _input = softwareBitmap;
                _inputSource = await softwareBitmap.ToSourceAsync();
                OnPropertyChanged(nameof(InputSource));
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
    }

    protected virtual string GetFactoryInputImagePath()
    {
        return DefaultFactoryImageFilePath;
    }

    public ICommand PickInputImageCommand => _pickInputImageCommand;

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

    private async Task SetInputImageAsync(SoftwareBitmap softwareBitmap)
    {
        await DispatcherQueue.EnqueueAsync(async () =>
        {
            _input = softwareBitmap;
            InputSource = await softwareBitmap.ToSourceAsync();
        });
    }
}
