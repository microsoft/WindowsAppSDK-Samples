if (ImageDescriptionGenerator.GetReadyState() == AIFeatureReadyState.NotReady)
{
    var imageDescriptionDeploymentOperation = ImageDescriptionGenerator.EnsureReadyAsync();
    await imageDescriptionDeploymentOperation;
}
ImageDescriptionGenerator _session = await ImageDescriptionGenerator.CreateAsync();

