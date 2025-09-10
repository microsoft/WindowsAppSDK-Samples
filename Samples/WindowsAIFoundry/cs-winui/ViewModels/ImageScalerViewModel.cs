using Microsoft.UI.Xaml.Media.Imaging;
using Microsoft.Windows.Storage.Pickers;
using System;
using System.Collections.Generic;
using System.IO;
using System.Threading.Tasks;
using System.Windows.Input;
using Windows.Graphics.Imaging;
using Windows.Storage.Streams;
using WindowsAISample.Models;
using WindowsAISample.Util;

namespace WindowsAISample.ViewModels;

internal partial class ImageScalerViewModel : InputImageViewModelBase<ImageScalerModel>
{
    private double _factor = 1.0;

    private readonly AsyncCommand<object?, SoftwareBitmapSource> _scaleSoftwareBitmapCommand;

    public ImageScalerViewModel(ImageScalerModel imageScalerSession)
        : base(imageScalerSession)
    {
        _scaleSoftwareBitmapCommand = new(
            async _ =>
            {
                if (Input is null)
                    return null!;

                var height = (int)(Input!.PixelHeight * Factor);
                var width = (int)(Input!.PixelWidth * Factor);
                var outputBitmap = Session.ScaleSoftwareBitmap(Input, width, height);

                return await DispatcherQueue.EnqueueAsync(async () =>
                {
                    // Start conversion to ImageSource early
                    var toSourceTask = outputBitmap.ToSourceAsync();

                    var picker = new FileSavePicker(App.WindowId)
                    {
                        SuggestedStartLocation = PickerLocationId.Downloads,
                        SuggestedFileName = $"scaled_{width}x{height}"
                    };
                    picker.FileTypeChoices.Add("PNG Image", new List<string> { ".png" });

                    var file = await picker.PickSaveFileAsync();

                    var outputImage = await toSourceTask;

                    if (file != null && !string.IsNullOrEmpty(file.Path))
                    {
                        await SaveBitmapToFileAsync(outputBitmap, file.Path);
                    }

                    return outputImage;
                });
            },
            _ => IsAvailable && Input is not null);
    }

    private static async Task SaveBitmapToFileAsync(SoftwareBitmap bitmap, string filePath)
    {
        using var fs = new FileStream(filePath, FileMode.Open, FileAccess.Write, FileShare.ReadWrite, 4096, true);
        using IRandomAccessStream stream = fs.AsRandomAccessStream();
        BitmapEncoder encoder = await BitmapEncoder.CreateAsync(BitmapEncoder.PngEncoderId, stream);
        encoder.SetSoftwareBitmap(bitmap);
        await encoder.FlushAsync();
    }

    public ICommand ScaleCommand => _scaleSoftwareBitmapCommand;

    public double Factor
    {
        get => _factor;
        set
        {
            if (SetField(ref _factor, value))
            {
                _scaleSoftwareBitmapCommand.FireCanExecuteChanged();
            }
        }
    }

    protected override void OnIsAvailableChanged()
    {
        _scaleSoftwareBitmapCommand.FireCanExecuteChanged();
    }
}
