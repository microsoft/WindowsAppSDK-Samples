if (ImageObjectExtractor.GetReadyState() == AIFeatureReadyState.NotReady)
{
    var objectExtractorDeploymentOperation = ImageObjectExtractor.EnsureReadyAsync();
    await objectExtractorDeploymentOperation;
}
