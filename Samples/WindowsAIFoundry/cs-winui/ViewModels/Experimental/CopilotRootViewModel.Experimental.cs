// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

namespace WindowsAISample.ViewModels;

/// <summary>
/// Experimental-only properties of <see cref="CopilotRootViewModel"/>.
///
/// This file lives under <c>ViewModels/Experimental/</c> and is excluded from
/// compilation when <c>IncludeExperimentalApis=false</c>, which removes these
/// properties from the type entirely in the stable build.
///
/// To add a new experimental feature, just add another property here with a
/// field initializer pointing at its Model.
/// </summary>
internal partial class CopilotRootViewModel
{
    public ImageForegroundExtractorViewModel ImageForegroundExtractor { get; }
        = new(new Models.ImageForegroundExtractorModel());

    public VideoScalerViewModel VideoScaler { get; }
        = new(new Models.VideoScalerModel());
}
