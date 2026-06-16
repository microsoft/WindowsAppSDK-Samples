// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using WindowsAISample.Models.Contracts;

namespace WindowsAISample.ViewModels;

/// <summary>
/// Root view model exposing each stable feature's ViewModel as a property,
/// so XAML in the stable Pages can bind through the inherited <see cref="rootFrame"/>
/// DataContext (e.g. <c>DataContext="{Binding LanguageModel}"</c>).
///
/// Extension pages (under <c>Extensions/</c>) set their own DataContext in
/// code-behind and do not appear here.
/// </summary>
internal class CopilotRootViewModel
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
