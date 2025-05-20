if (ImageScaler.GetReadyState() == AIFeatureReadyState.NotReady)
{
    var imageScalerDeploymentOperation = ImageScaler.EnsureReadyAsync();
    await imageScalerDeploymentOperation;
}
ImageScaler _session = await ImageScaler.CreateAsync();
