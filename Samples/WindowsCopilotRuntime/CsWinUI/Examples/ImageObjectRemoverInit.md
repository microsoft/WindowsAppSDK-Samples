IProgress<PackageDeploymentProgress> progress;
if (ImageObjectRemover.GetReadyState() == AIFeatureReadyState.EnsureNeeded)
{
    var imageObjectRemoverDeploymentOperationAsync = ImageObjectRemover.EnsureReadyAsync();
    await imageObjectRemoverDeploymentOperationAsync;
}
ImageObjectRemover model = await ImageObjectRemover.CreateAsync();
