IProgress<PackageDeploymentProgress> progress;
if (ImageObjectRemover.GetReadyState() == AIFeatureReadyState.EnsureNeeded)
{
    var imageObjectRemoverDeploymentOperationAsync = ImageObjectRemover.EnsureReadyAsync();
    imageObjectRemoverDeploymentOperationAsync.Progress = (_, packageDeploymentProgress) =>
    {
        progress.Report(packageDeploymentProgress);
    };
    await imageObjectRemoverDeploymentOperationAsync;
}
else
{
    progress.Report(100.0);
}
ImageObjectRemover model = await ImageObjectRemover.CreateAsync();
