// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using WindowsCopilotRuntimeSample.Models.Contracts;
using WindowsCopilotRuntimeSample.Util;
using Microsoft.Graphics.Imaging;
using System.Windows.Input;
using WindowsCopilotRuntimeSample.Models;

namespace WindowsCopilotRuntimeSample.ViewModels;

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
                imageBuffer = ImageBuffer.CreateCopyFromBitmap(Input);
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