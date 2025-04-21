var languageModelOptions = new LanguageModelOptions {
    Skill = LanguageModelSkill.Rewrite,
    Temp =  0.9f,
    Top_p = 0.9f,
    Top_k = 40
};

IProgress<string> progress;
//uses default value for content filtering
var languageModelResponseWithProgress = model.GenerateResponseWithProgressAsync(languageModelOptions, prompt);  
languageModelRepsonseWithProgress.Progress = (_, generationProgress) =>
{
    progress.Report(generationProgress);
};
string response = (await languageModelResponseWithProgress).Response;
