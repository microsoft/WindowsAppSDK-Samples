if (ImageDescriptionGenerator.GetReadyState() == AIFeatureReadyState.EnsureNeeded)
{
    var imageDescriptionDeploymentOperation = ImageDescriptionGenerator.EnsureReadyAsync();
    imageDescriptionDeploymentOperation.Progress = (_, modelDeploymentProgress) =>
    {
        progress.Report(modelDeploymentProgress);
    };
    using var _ = cancellationToken.Register(() => imageDescriptionDeploymentOperation.Cancel());
    await imageDescriptionDeploymentOperation;
}
else
{
    progress.Report(100.0);
}
ImageDescriptionGenerator _session = await ImageDescriptionGenerator.CreateAsync();

