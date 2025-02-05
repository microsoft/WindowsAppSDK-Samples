IProgress<PackageDeploymentProgress> progress;
if (!ImageDescriptionGenerator.IsAvailable())
{
    var imageDescriptionDeploymentOperation = ImageDescriptionGenerator.MakeAvailableAsync();
    imageDescriptionDeploymentOperation.Progress = (_, packageDeploymentProgress) =>
    {
        progress.Report(packageDeploymentProgress);
    };
    using var _ = cancellationToken.Register(() => imageDescriptionDeploymentOperation.Cancel());
    await imageDescriptionDeploymentOperation;
}
else
{
    progress.Report(new PackageDeploymentProgress(PackageDeploymentProgressStatus.CompletedSuccess, 100.0));
}
ImageDescriptionGenerator model = await ImageDescriptionGenerator.CreateAsync();

