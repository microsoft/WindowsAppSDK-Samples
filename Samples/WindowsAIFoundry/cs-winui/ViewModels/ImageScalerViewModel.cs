// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using WindowsAISampleSample.Models.Contracts;
using WindowsAISampleSample.Util;
using Microsoft.UI.Xaml.Media.Imaging;
using System.Windows.Input;
using Windows.Graphics.Imaging;
using WindowsAISampleSample.Models;

namespace WindowsAISampleSample.ViewModels;

internal partial class ImageScalerViewModel : InputImageViewModelBase<ImageScalerModel>
{
    private double _factor = 1.0f;

    private readonly AsyncCommand<(SoftwareBitmap, int, int), SoftwareBitmapSource> _scaleSoftwareBitmapCommand;

    public ImageScalerViewModel(ImageScalerModel imageScalerSession)
    : base(imageScalerSession)
    {
        _scaleSoftwareBitmapCommand = new(
            async _ =>
            {
                var height = (int)(Input!.PixelHeight * Factor);
                var width = (int)(Input!.PixelWidth * Factor);
                var outputBitmap = Session.ScaleSoftwareBitmap(Input!, width, height);
                return await DispatcherQueue.EnqueueAsync(async () =>
                {
                    return await outputBitmap.ToSourceAsync();
                });
            },
            (_) => IsAvailable && Input is not null);
    }

    public ICommand ScaleCommand => _scaleSoftwareBitmapCommand;

    public double Factor
    {
        get
        {
            return _factor;
        }
        set
        {
            SetField(ref _factor, value);
            _scaleSoftwareBitmapCommand.FireCanExecuteChanged();
        }
    }

    protected override void OnIsAvailableChanged()
    {
        _scaleSoftwareBitmapCommand.FireCanExecuteChanged();
    }
}
