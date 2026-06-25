// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using WindowsAISample.Ext.VideoScaler.Models;
using WindowsAISample.Util;
using WindowsAISample.ViewModels;
using Microsoft.UI.Xaml.Media.Imaging;
using System.Windows.Input;
using Windows.Media;
using System.Threading.Tasks;
using Microsoft.UI.Dispatching;

namespace WindowsAISample.Ext.VideoScaler.ViewModels;

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