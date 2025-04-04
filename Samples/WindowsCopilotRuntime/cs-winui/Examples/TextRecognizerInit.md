if (TextRecognizer.GetReadyState() == AIFeatureReadyState.EnsureNeeded)
{
    var textRecognizerDeploymentOperation = TextRecognizer.EnsureReadyAsync();
    textRecognizerDeploymentOperation.Progress = (_, modelDeploymentProgress) =>
    {
        progress.Report(modelDeploymentProgress);
    };
    using var _ = cancellationToken.Register(() => textRecognizerDeploymentOperation.Cancel());
    await textRecognizerDeploymentOperation;
}
else
{
    progress.Report(100.0);
}
TextRecognizer _session = await TextRecognizer.CreateAsync();
