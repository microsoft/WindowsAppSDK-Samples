// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using WindowsAISample.Models.Contracts;

namespace WindowsAISample.ViewModels;

/// <summary>
/// Our root model to expose all the AI Fabric API's.
///
/// This class is intentionally <c>partial</c>: feature properties that depend
/// on the experimental Windows App SDK live in a sibling file under
/// <c>ViewModels/Experimental/</c>, which the project excludes from the build
/// when <c>IncludeExperimentalApis=false</c>. Each property uses a field
/// initializer, so adding or promoting a feature never touches this file.
/// </summary>
internal partial class CopilotRootViewModel
{
    public LanguageModelViewModel LanguageModel { get; }
        = new(new Models.LanguageModelModel());

    public TextRecognizerViewModel TextRecognizer { get; }
        = new(new Models.TextRecognizerModel());

    public ImageScalerViewModel ImageScaler { get; }
        = new(new Models.ImageScalerModel());

    public ImageDescriptionViewModel ImageDescriptionGenerator { get; }
        = new(new Models.ImageDescriptionModel());

    public ImageObjectExtractorViewModel ImageObjectExtractor { get; }
        = new(new Models.ImageObjectExtractorModel());

    public ImageObjectRemoverViewModel ImageObjectRemover { get; }
        = new(new Models.ImageObjectRemoverModel());
}
