IProgress<PackageDeploymentProgress> progress;
if (!ImageObjectExtractor.IsAvailable())
{
    var objectExtractorDeploymentOperationAsync = ImageObjectExtractor.MakeAvailableAsync();
    objectExtractorDeploymentOperationAsync.Progress = (_, packageDeploymentProgress) =>
    {
        progress.Report(packageDeploymentProgress);
    };
    await objectExtractorDeploymentOperationAsync;
}
else
{
    progress.Report(new PackageDeploymentProgress(PackageDeploymentProgressStatus.CompletedSuccess, 100.0));
}
