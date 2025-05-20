// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using WindowsAISample.Models.Contracts;
using WindowsAISample.Util;
using Microsoft.Graphics.Imaging;
using System.Windows.Input;
using Windows.Graphics.Imaging;
using WindowsAISample.Models;
using System.Collections;
using System.Linq;
using Microsoft.Windows.AI.Imaging;

namespace WindowsAISample.ViewModels;

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
