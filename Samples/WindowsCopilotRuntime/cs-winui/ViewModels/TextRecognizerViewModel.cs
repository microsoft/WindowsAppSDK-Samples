// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using WindowsCopilotRuntimeSample.Models.Contracts;
using WindowsCopilotRuntimeSample.Util;
using Microsoft.Graphics.Imaging;
using Microsoft.Windows.AI.Imaging;
using System.Windows.Input;
using Windows.Graphics.Imaging;
using WindowsCopilotRuntimeSample.Models;
using System.Collections;
using System.Linq;

namespace WindowsCopilotRuntimeSample.ViewModels;

internal partial class TextRecognizerViewModel : InputImageViewModelBase<TextRecognizerModel>
{
    private const string DefaultFactoryImageFilePath = "Assets/ocr-text.jpg";
    private readonly AsyncCommand<SoftwareBitmap/*, TextRecognizerOptions*/, string> _recognizeTextFromImageCommand;

    public TextRecognizerViewModel(TextRecognizerModel textRecognizerSession)
    : base(textRecognizerSession)
    {
        _recognizeTextFromImageCommand = new(
            _ =>
            {
                //var options = new TextRecognizerOptions();
                //options.MaxAnalysisSize = new global::Windows.Graphics.SizeInt32(1000, 1000);
                //options.OrientationDetection = OrientationDetectionOptions.None;

                using var imageBuffer = ImageBuffer.CreateForSoftwareBitmap(Input);
                RecognizedText text = Session.RecognizeTextFromImage(imageBuffer); //, options);
                RecognizedLine[] lines = text.Lines;
                string outputString = string.Join(" ", lines.Select(line => line.Text));
                return outputString;
            },
            (_) => IsAvailable && Input is not null);
    }

    public ICommand RecognizeCommand => _recognizeTextFromImageCommand;

    protected override void OnIsAvailableChanged()
    {
        _recognizeTextFromImageCommand.FireCanExecuteChanged();
    }

    protected override string GetFactoryInputImagePath()
    {
        return DefaultFactoryImageFilePath;
    }
}
