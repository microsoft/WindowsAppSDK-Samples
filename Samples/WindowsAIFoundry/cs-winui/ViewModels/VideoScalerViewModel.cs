// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using WindowsAISample.Models.Contracts;
using WindowsAISample.Util;
using Microsoft.UI.Xaml.Media.Imaging;
using System.Windows.Input;
using Windows.Media;
using WindowsAISample.Models;
using System.Threading.Tasks;
using Microsoft.UI.Dispatching;

namespace WindowsAISample.ViewModels;

internal partial class VideoScalerViewModel : InputImageViewModelBase<VideoScalerModel>
{
    private readonly AsyncCommand<(VideoFrame, int, int), SoftwareBitmapSource> _scaleVideoFrameCommand;

    public VideoScalerViewModel(VideoScalerModel videoScalerSession)
    : base(videoScalerSession)
    {
        _scaleVideoFrameCommand = new(
            async _ =>
            { 
                var softwareBitmap = Session.ScaleVideoFrame(Input!);
                return await DispatcherQueue.EnqueueAsync(async () =>
                {
                    return await softwareBitmap.ToSourceAsync();
                });
            },
            (_) => IsAvailable && Input is not null);
    }

    public ICommand ScaleCommand => _scaleVideoFrameCommand;

    protected override void OnIsAvailableChanged()
    {
        _scaleVideoFrameCommand.FireCanExecuteChanged();
    }
}