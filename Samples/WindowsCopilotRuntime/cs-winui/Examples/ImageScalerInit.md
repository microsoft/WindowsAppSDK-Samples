if (ImageScaler.GetReadyState() == AIFeatureReadyState.EnsureNeeded)
{
    var imageScalerDeploymentOperation = ImageScaler.EnsureReadyAsync();
    await imageScalerDeploymentOperation;
}
ImageScaler _session = await ImageScaler.CreateAsync();
