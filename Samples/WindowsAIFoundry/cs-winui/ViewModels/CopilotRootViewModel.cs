// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using WindowsAISample.Models.Contracts;

namespace WindowsAISample.ViewModels;

/// <summary>
/// Our root model to expose all the AI Fabric API's
/// </summary>
internal class CopilotRootViewModel
{
    internal CopilotRootViewModel()
    {
        LanguageModel = new(new Models.LanguageModelModel());
        TextRecognizer = new(new Models.TextRecognizerModel());
        ImageScaler = new(new Models.ImageScalerModel());
        ImageObjectExtractor = new(new Models.ImageObjectExtractorModel());
        ImageDescriptionGenerator = new(new Models.ImageDescriptionModel());
        VideoScaler = new(new Models.VideoScalerModel());
    }

    public LanguageModelViewModel LanguageModel { get; }

    public TextRecognizerViewModel TextRecognizer { get; }

    public ImageScalerViewModel ImageScaler { get; }

    public ImageDescriptionViewModel ImageDescriptionGenerator { get; }

    public ImageObjectExtractorViewModel ImageObjectExtractor { get; }

    public VideoScalerViewModel VideoScaler { get; }
}