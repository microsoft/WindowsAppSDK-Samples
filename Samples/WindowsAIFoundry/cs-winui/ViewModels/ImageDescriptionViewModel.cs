﻿// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using WindowsAISample.Models.Contracts;
using WindowsAISample.Util;
using Microsoft.Graphics.Imaging;
using System.Windows.Input;
using WindowsAISample.Models;

namespace WindowsAISample.ViewModels;

partial class ImageDescriptionViewModel : InputImageViewModelBase<ImageDescriptionModel>
{

    private readonly AsyncCommand<ImageBuffer, string> _imageDescriptionCommand;
    private ImageBuffer? imageBuffer;

    public ImageDescriptionViewModel(ImageDescriptionModel imageDescriptionSession)
    : base(imageDescriptionSession)
    {
        _imageDescriptionCommand = new(
            async _ =>
            {
                imageBuffer = ImageBuffer.CreateForSoftwareBitmap(Input);
                return await Session.DescribeImage(imageBuffer);
            },
            (_) => IsAvailable && Input is not null);
    }
    public ICommand ImageDescriptionCommand => _imageDescriptionCommand;
    

    protected override void OnIsAvailableChanged()
    {
        _imageDescriptionCommand.FireCanExecuteChanged();
    }
    
}