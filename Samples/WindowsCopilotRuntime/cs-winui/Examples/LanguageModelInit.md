if (LanguageModel.GetReadyState() == AIFeatureReadyState.EnsureNeeded)
{
    var languageModelDeploymentOperation = LanguageModel.EnsureReadyAsync();
    languageModelDeploymentOperation.Progress = (_, modelDeploymentProgress) =>
    {
        progress.Report(modelDeploymentProgress);
    };
    using var _ = cancellationToken.Register(() => languageModelDeploymentOperation.Cancel());
    await languageModelDeploymentOperation;
}
else
{
    progress.Report(100.0);
}
LanguageModel _session = await LanguageModel.CreateAsync();
