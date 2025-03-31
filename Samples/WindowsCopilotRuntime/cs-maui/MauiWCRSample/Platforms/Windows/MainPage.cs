using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Devices.Scanners;
using Microsoft.Graphics.Imaging;
using Microsoft.Maui.Platform;
using Microsoft.UI.Xaml.Media.Imaging;
using Windows.Graphics.Imaging;
using Windows.Storage.Streams;
using Microsoft.Windows.AI.Generative;

namespace MauiWindowsCopilotRuntimeSample // extending the partial class MainPage with the platform-specific implementation
{
    partial class MainPage
    {
        partial void ShowImageScalingPanelIfSupported()
        {
            ShowImageScalingPanelIfSupportedImpl();
        }

        private async void ShowImageScalingPanelIfSupportedImpl()
        {
            try
            {
                if (!ImageScaler.IsAvailable())
                {
                    System.Diagnostics.Debug.WriteLine("Calling ImageScaler.MakeAvailableAsync");
                    var op = await ImageScaler.MakeAvailableAsync();
                    System.Diagnostics.Debug.WriteLine($"ImageScaler.MakeAvailableAsync completed with Status: {op.Status}");
                }

                if (ImageScaler.IsAvailable())
                {
                    // Image scaling is supported, so show the content
                    imageScalingPanel.IsVisible = true;
                }
            }
            catch (Exception ex)
            {
                // Log the exception
                System.Diagnostics.Debug.WriteLine("Unable to use ImageScaler: " + ex.Message);
            }
        }

        partial void ScaleImage(double scale)
        {
            DoScaleImage(scale);
        }

        private async void DoScaleImage(double scale)
        {
            // Load the original image
            var resourceManager = new Microsoft.Windows.ApplicationModel.Resources.ResourceManager();
            var resource = resourceManager.MainResourceMap.GetValue("ms-resource:///Files/enhance.png");
            if (resource.Kind == Microsoft.Windows.ApplicationModel.Resources.ResourceCandidateKind.FilePath)
            {
                // Load as a SoftwareBitmap
                var file = await Windows.Storage.StorageFile.GetFileFromPathAsync(resource.ValueAsString);
                var fileStream = await file.OpenStreamForReadAsync();

                var decoder = await BitmapDecoder.CreateAsync(fileStream.AsRandomAccessStream());
                var softwareBitmap = await decoder.GetSoftwareBitmapAsync();
                int origWidth = softwareBitmap.PixelWidth;
                int origHeight = softwareBitmap.PixelHeight;

                SoftwareBitmap finalImage;
                if (scale == 0.0)
                {
                    // just show the original image
                    finalImage = softwareBitmap;
                }
                else
                {
                    // Scale the image to be the exact pixel size of the element displaying it
                    if (!ImageScaler.IsAvailable())
                    {
                        var op = await ImageScaler.MakeAvailableAsync();
                    }

                    ImageScaler imageScaler = await ImageScaler.CreateAsync();

                    double imageScale = scale;
                    if (imageScale > imageScaler.MaxSupportedScaleFactor)
                    {
                        imageScale = imageScaler.MaxSupportedScaleFactor;
                    }
                    System.Diagnostics.Debug.WriteLine($"Scaling to {imageScale}x...");

                    int newHeight = (int)(origHeight * imageScale);
                    int newWidth = (int)(origWidth * imageScale);
                    finalImage = imageScaler.ScaleSoftwareBitmap(softwareBitmap, newWidth, newHeight);
                }

                // Display the scaled image. The if/else here shows two different approaches to do this.
                var mauiContext = scaledImage.Handler?.MauiContext;
                if (mauiContext != null)
                {
                    // set the SoftwareBitmap as the source of the Image control
                    var imageToShow = finalImage;
                    if (imageToShow.BitmapPixelFormat != BitmapPixelFormat.Bgra8 ||
                        imageToShow.BitmapAlphaMode == BitmapAlphaMode.Straight)
                    {
                        // SoftwareBitmapSource only supports Bgra8 and doesn't support Straight alpha mode, so convert
                        imageToShow = SoftwareBitmap.Convert(imageToShow, BitmapPixelFormat.Bgra8, BitmapAlphaMode.Premultiplied);
                    }
                    var softwareBitmapSource = new SoftwareBitmapSource();
                    _ = softwareBitmapSource.SetBitmapAsync(imageToShow);
                    var nativeScaledImageView = (Microsoft.UI.Xaml.Controls.Image)scaledImage.ToPlatform(mauiContext);
                    nativeScaledImageView.Source = softwareBitmapSource;
                }
                else
                {
                    // An alternative approach is to encode the image so a stream can be handed
                    // to the Maui ImageSource.

                    // Note: There's no "using(...)" here, since this stream needs to be kept alive for the image to be displayed
                    var scaledStream = new InMemoryRandomAccessStream();
                    {
                        BitmapEncoder encoder = await BitmapEncoder.CreateAsync(BitmapEncoder.PngEncoderId, scaledStream);
                        encoder.SetSoftwareBitmap(finalImage);
                        await encoder.FlushAsync();
                        scaledImage.Source = ImageSource.FromStream(() => scaledStream.AsStream());
                    }
                }
            }
        }

        partial void ShowTextGenerationPanelIfSupported()
        {
            ShowTextGenerationPanelIfSupportedImpl();
        }

        private async void ShowTextGenerationPanelIfSupportedImpl()
        {
            try
            {
                if (!LanguageModel.IsAvailable())
                {
                    System.Diagnostics.Debug.WriteLine("Calling LanguageModel.MakeAvailableAsync");
                    var op = await LanguageModel.MakeAvailableAsync();
                    System.Diagnostics.Debug.WriteLine($"LanguageModel.MakeAvailableAsync completed with Status: {op.Status}");
                }

                if (LanguageModel.IsAvailable())
                {
                    // LanguageModel is supported, so show the content and set a default prompt
                    textGenerationPanel.IsVisible = true;
                    entryPrompt.Text = "Introduce yourself in detail.";
                }
            }
            catch (Exception ex)
            {
                // Log the exception
                System.Diagnostics.Debug.WriteLine("Unable to use LanguageModel: " + ex.Message);
            }
        }


        partial void GenerateTextFromEntryPrompt()
        {
            DoGenerateTextFromEntryPrompt();
        }

        private async void DoGenerateTextFromEntryPrompt()
        {
            if (!LanguageModel.IsAvailable())
            {
                answer.Text = "Making LanguageModel available...";
                var op = await LanguageModel.MakeAvailableAsync();
            }

            answer.Text = "Preparing LanguageModel...";
            using LanguageModel languageModel = await LanguageModel.CreateAsync();

            string prompt = entryPrompt.Text;

            answer.Text = "Generating response...";
            Windows.Foundation.AsyncOperationProgressHandler<LanguageModelResponse, string>
            progressHandler = (asyncInfo, delta) =>
            {
                System.Diagnostics.Debug.WriteLine($"Progress: {delta}");
                var fullResponse = asyncInfo.GetResults().Response;
                System.Diagnostics.Debug.WriteLine($"Response so far: {fullResponse}");
                var newText = "Q: " + prompt + "\nA:" + fullResponse;
                answer.Dispatcher.Dispatch(() => { answer.Text = newText; });
            };

            var asyncOp = languageModel.GenerateResponseWithProgressAsync(prompt);

            asyncOp.Progress = progressHandler;

            var result = await asyncOp;
            System.Diagnostics.Debug.WriteLine("DONE: " + result.Response);
        }
    }
}
