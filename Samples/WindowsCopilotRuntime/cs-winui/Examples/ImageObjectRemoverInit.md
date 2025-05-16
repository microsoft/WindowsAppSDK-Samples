IProgress<PackageDeploymentProgress> progress;
if (ImageObjectRemover.GetReadyState() == AIFeatureReadyState.NotReady)
{
    var imageObjectRemoverDeploymentOperationAsync = ImageObjectRemover.EnsureReadyAsync();
    await imageObjectRemoverDeploymentOperationAsync;
}
ImageObjectRemover model = await ImageObjectRemover.CreateAsync();
