IProgress<PackageDeploymentProgress> progress;
if (!ImageScaler.IsAvailable())
{
    var imageScalerDeploymentOperationAsync = ImageScaler.MakeAvailableAsync();
    imageScalerDeploymentOperationAsync.Progress = (_, packageDeploymentProgress) =>
    {
        progress.Report(packageDeploymentProgress);
    };
    await imageScalerDeploymentOperationAsync;
}
else
{
    progress.Report(new PackageDeploymentProgress(PackageDeploymentProgressStatus.CompletedSuccess, 100.0));
}
ImageScaler model = await ImageScaler.CreateAsync();
