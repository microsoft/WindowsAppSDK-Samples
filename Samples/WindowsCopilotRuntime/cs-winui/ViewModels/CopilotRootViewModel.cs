// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using Microsoft.Windows.AI.Generative;
using WindowsCopilotRuntimeSample.Models.Contracts;

namespace WindowsCopilotRuntimeSample.ViewModels;

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
    }

    public LanguageModelViewModel LanguageModel { get; }

    public TextRecognizerViewModel TextRecognizer { get; }

    public ImageScalerViewModel ImageScaler { get; }

    public ImageDescriptionViewModel ImageDescriptionGenerator { get; }

    public ImageObjectExtractorViewModel ImageObjectExtractor { get; }
}
