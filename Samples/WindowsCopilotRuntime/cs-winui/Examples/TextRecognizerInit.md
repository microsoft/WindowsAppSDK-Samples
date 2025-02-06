IProgress<PackageDeploymentProgress> progress;
if (!TextRecognizer.IsAvailable())
{
    var textRecognizerDeploymentOperationAsync = TextRecognizer.MakeAvailableAsync();
    textRecognizerDeploymentOperationAsync.Progress = (_, packageDeploymentProgress) =>
    {
        progress.Report(packageDeploymentProgress);
    };
    await textRecognizerDeploymentOperationAsync;
}
else
{
    progress.Report(new PackageDeploymentProgress(PackageDeploymentProgressStatus.CompletedSuccess, 100.0));
}
TextRecognizer model = await TextRecognizer.CreateAsync();
