// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using WindowsCopilotRuntimeSample.Models.Contracts;
using WindowsCopilotRuntimeSample.Util;
using Microsoft.Graphics.Imaging;
using System.Windows.Input;
using Windows.Graphics.Imaging;
using WindowsCopilotRuntimeSample.Models;
using System.Collections;
using System.Linq;
using Microsoft.Windows.AI.Imaging;

namespace WindowsCopilotRuntimeSample.ViewModels;

internal partial class TextRecognizerViewModel : InputImageViewModelBase<TextRecognizerModel>
{
    private const string DefaultFactoryImageFilePath = "Assets/ocr-text.jpg";
    private readonly AsyncCommand<SoftwareBitmap, string> _recognizeTextFromImageCommand;

    public TextRecognizerViewModel(TextRecognizerModel textRecognizerSession)
    : base(textRecognizerSession)
    {
        _recognizeTextFromImageCommand = new(
            _ =>
            {
               

                using var imageBuffer = ImageBuffer.CreateForSoftwareBitmap(Input!);
                RecognizedText text = Session.RecognizeTextFromImage(imageBuffer);
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
