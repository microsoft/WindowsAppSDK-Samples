if (ImageForegroundExtractor.GetReadyState() == AIFeatureReadyState.NotReady)
{
    await ImageForegroundExtractor.EnsureReadyAsync();
}