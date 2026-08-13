var catalog = ExecutionProviderCatalog.GetDefault();
await catalog.EnsureAndRegisterCertifiedAsync();

if (VideoScaler.GetReadyState() == AIFeatureReadyState.NotReady)
{
    var videoScalerDeploymentOperation = VideoScaler.EnsureReadyAsync();
    await videoScalerDeploymentOperation;
}
VideoScaler _session = await VideoScaler.CreateAsync();
