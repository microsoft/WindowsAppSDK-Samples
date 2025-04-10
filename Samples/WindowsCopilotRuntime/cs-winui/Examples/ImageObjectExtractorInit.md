if (ImageObjectExtractor.GetReadyState() == AIFeatureReadyState.EnsureNeeded)
{
    var objectExtractorDeploymentOperation = ImageObjectExtractor.EnsureReadyAsync();
    await objectExtractorDeploymentOperation;
}
