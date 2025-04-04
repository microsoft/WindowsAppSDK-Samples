if (ImageScaler.GetReadyState() == AIFeatureReadyState.EnsureNeeded)
{
    var imageScalerDeploymentOperation = ImageScaler.EnsureReadyAsync();
    imageScalerDeploymentOperation.Progress = (_, modelDeploymentProgress) =>
    {
        progress.Report(modelDeploymentProgress);
    };
    using var _ = cancellationToken.Register(() => imageScalerDeploymentOperation.Cancel());
    await imageScalerDeploymentOperation;
}
else
{
    progress.Report(100.0);
}
ImageScaler _session = await ImageScaler.CreateAsync();
