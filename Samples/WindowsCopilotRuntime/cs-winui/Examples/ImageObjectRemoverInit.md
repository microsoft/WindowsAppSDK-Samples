IProgress<PackageDeploymentProgress> progress;
if (!ImageObjectRemover.IsAvailable())
{
    var imageObjectRemoverDeploymentOperationAsync = ImageObjectRemover.MakeAvailableAsync();
    imageObjectRemoverDeploymentOperationAsync.Progress = (_, packageDeploymentProgress) =>
    {
        progress.Report(packageDeploymentProgress);
    };
    await imageObjectRemoverDeploymentOperationAsync;
}
else
{
    progress.Report(new PackageDeploymentProgress(PackageDeploymentProgressStatus.CompletedSuccess, 100.0));
}
ImageObjectRemover model = await ImageObjectRemover.CreateAsync();
