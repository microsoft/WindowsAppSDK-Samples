if (TextRecognizer.GetReadyState() == AIFeatureReadyState.NotReady)
{
    var textRecognizerDeploymentOperation = TextRecognizer.EnsureReadyAsync();
    await textRecognizerDeploymentOperation;
}
TextRecognizer _session = await TextRecognizer.CreateAsync();
