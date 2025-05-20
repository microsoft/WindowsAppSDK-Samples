if (ImageDescriptionGenerator.GetReadyState() == AIFeatureReadyState.EnsureNeeded)
{
    var imageDescriptionDeploymentOperation = ImageDescriptionGenerator.EnsureReadyAsync();
    await imageDescriptionDeploymentOperation;
}
ImageDescriptionGenerator _session = await ImageDescriptionGenerator.CreateAsync();

