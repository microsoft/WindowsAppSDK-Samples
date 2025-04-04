if (ImageObjectExtractor.GetReadyState() == AIFeatureReadyState.EnsureNeeded)
{
    var objectExtractorDeploymentOperation = ImageObjectExtractor.EnsureReadyAsync();
    objectExtractorDeploymentOperation.Progress = (_, modelDeploymentProgress) =>
    {
        progress.Report(modelDeploymentProgress);
    };
    using var _ = cancellationToken.Register(() => objectExtractorDeploymentOperation.Cancel());
    await objectExtractorDeploymentOperation;
}
else
{
    progress.Report(100.0);
}
