if (TextRecognizer.GetReadyState() == AIFeatureReadyState.EnsureNeeded)
{
    var textRecognizerDeploymentOperation = TextRecognizer.EnsureReadyAsync();
    await textRecognizerDeploymentOperation;
}
TextRecognizer _session = await TextRecognizer.CreateAsync();
