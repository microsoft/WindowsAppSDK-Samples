if (LanguageModel.GetReadyState() == AIFeatureReadyState.EnsureNeeded)
{
    var languageModelDeploymentOperation = LanguageModel.EnsureReadyAsync();
    await languageModelDeploymentOperation;
}
LanguageModel _session = await LanguageModel.CreateAsync();
