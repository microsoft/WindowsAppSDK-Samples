if (LanguageModel.GetReadyState() == AIFeatureReadyState.NotReady)
{
    var languageModelDeploymentOperation = LanguageModel.EnsureReadyAsync();
    await languageModelDeploymentOperation;
}
LanguageModel _session = await LanguageModel.CreateAsync();
