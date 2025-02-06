var languageModelOptions = new LanguageModelOptions();

var contentFilterOptions = new ContentFilterOptions();
var languageModelContext = model.CreateContext(context, contentFilterOptions);

IProgress<string> progress;
var languageModelResponseWithProgress = model.GenerateResponseWithProgressAsync(languageModelOptions, prompt, contentFilterOptions, languageModelContext);
languageModelRepsonseWithProgress.Progress = (_, generationProgress) =>
{
    progress.Report(generationProgress);
};
string response = (await languageModelResponseWithProgress).Response;
