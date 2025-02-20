IProgress<PackageDeploymentProgress> progress;
if (!LanguageModel.IsAvailable())
{
    var languageModelDeploymentOperationAsync = LanguageModel.MakeAvailableAsync();
    languageModelDeploymentOperationAsync.Progress = (_, packageDeploymentProgress) =>
    {
        progress.Report(packageDeploymentProgress);
    };
    await languageModelDeploymentOperationAsync;
}
else
{
    progress.Report(new PackageDeploymentProgress(PackageDeploymentProgressStatus.CompletedSuccess, 100.0));
}
LanguageModel model = await LanguageModel.CreateAsync();
