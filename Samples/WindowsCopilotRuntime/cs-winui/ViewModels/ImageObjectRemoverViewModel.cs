// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//using WindowsCopilotRuntimeSample.Models.Contracts;
//using WindowsCopilotRuntimeSample.Util;
//using Microsoft.UI.Xaml.Media.Imaging;
//using System.Windows.Input;
//using Windows.Graphics.Imaging;
//using WindowsCopilotRuntimeSample.Models;

//namespace WindowsCopilotRuntimeSample.ViewModels;

//internal partial class ImageObjectRemoverViewModel : InputImageViewModelBase<ImageObjectRemoverModel>
//{
//    private readonly AsyncCommand<(SoftwareBitmap, int, int), SoftwareBitmapSource> _removeFromSoftwareBitmapCommand;

//    public ImageObjectRemoverViewModel(ImageObjectRemoverModel imageObjectRemoverSession)
//    : base(imageObjectRemoverSession)
//    {
//        _removeFromSoftwareBitmapCommand = new(
//            async _ =>
//            {
//                var outputBitmap = Session.RemoveFromSoftwareBitmap(Input!, Mask!);
//                return await DispatcherQueue.EnqueueAsync(async () =>
//                {
//                    return await outputBitmap.ToSourceAsync();
//                });
//            },
//            (_) => IsAvailable && Input is not null);
//    }

//    public ICommand RemoveObjectCommand => _removeFromSoftwareBitmapCommand;

//    protected override void OnIsAvailableChanged()
//    {
//        _removeFromSoftwareBitmapCommand.FireCanExecuteChanged();
//    }
//}
