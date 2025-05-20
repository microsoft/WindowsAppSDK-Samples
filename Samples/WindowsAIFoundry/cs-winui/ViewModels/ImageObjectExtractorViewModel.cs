// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using WindowsAISampleSample.Models.Contracts;
using WindowsAISampleSample.Util;
using Microsoft.Graphics.Imaging;
using Microsoft.UI.Xaml.Media.Imaging;
using System;
using System.Collections.Generic;
using System.Windows.Input;
using Windows.Graphics;
using Windows.Graphics.Imaging;
using WindowsAISampleSample.Models;
using Microsoft.Windows.AI.Imaging;

namespace WindowsAISampleSample.ViewModels;

partial class ImageObjectExtractorViewModel : InputImageViewModelBase<ImageObjectExtractorModel>
{
    private readonly IList<PointInt32> _selectionPoints = new List<PointInt32>();
    private readonly AsyncCommand<SoftwareBitmap, SoftwareBitmapSource> _applyImageObjectExtractorCommand;

    public ImageObjectExtractorViewModel(ImageObjectExtractorModel imageObjectExtractorSession)
    : base(imageObjectExtractorSession)
    {
        _applyImageObjectExtractorCommand = new(
            async _ =>
            {
                if (Input == null)
                {
                    throw new InvalidOperationException();
                }


                var outputMask = await Session.ApplyImageObjectExtractorAsync(
                    Input,
                    new ImageObjectExtractorHint(
                        includeRects: null,
                        includePoints: SelectionPoints,
                        excludePoints: null
                    )
                );
                var outputBitmap = Input.ApplyMask(outputMask);

                return await DispatcherQueue.EnqueueAsync(async () =>
                {
                    return await outputBitmap.ToSourceAsync();
                });
            },
            (_) => IsAvailable && Input is not null);
    }

    public ICommand ExtractObjectsCommand => _applyImageObjectExtractorCommand;

    public IList<PointInt32> SelectionPoints
    {
        get
        {
            return _selectionPoints;
        }
    }

    protected override void OnIsAvailableChanged()
    {
        _applyImageObjectExtractorCommand.FireCanExecuteChanged();
    }
}