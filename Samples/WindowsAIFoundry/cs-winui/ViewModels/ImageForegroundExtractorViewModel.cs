// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using WindowsAISample.Models.Contracts;
using WindowsAISample.Util;
using Microsoft.UI.Xaml.Media.Imaging;
using System;
using System.Windows.Input;
using Windows.Graphics.Imaging;
using WindowsAISample.Models;

namespace WindowsAISample.ViewModels;

internal partial class ImageForegroundExtractorViewModel : InputImageViewModelBase<ImageForegroundExtractorModel>
{
    private readonly AsyncCommand<SoftwareBitmap, SoftwareBitmapSource> _extractForegroundCommand;

    public ImageForegroundExtractorViewModel(ImageForegroundExtractorModel imageForegroundExtractorSession)
    : base(imageForegroundExtractorSession)
    {
        _extractForegroundCommand = new(
            async _ =>
            {
                if (Input == null)
                {
                    throw new InvalidOperationException();
                }

                var foregroundMask = await Session.ExtractForegroundMaskAsync(Input);
                var outputBitmap = Input.ApplyMask(foregroundMask);

                return await DispatcherQueue.EnqueueAsync(async () =>
                {
                    return await outputBitmap.ToSourceAsync();
                });
            },
            (_) => IsAvailable && Input is not null);
    }

    public ICommand ExtractForegroundCommand => _extractForegroundCommand;

    protected override void OnIsAvailableChanged()
    {
        _extractForegroundCommand.FireCanExecuteChanged();
    }
}
