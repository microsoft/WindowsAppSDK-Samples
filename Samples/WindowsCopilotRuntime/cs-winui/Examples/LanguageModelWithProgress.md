IProgress<string> progress;

var languageModelResponseWithProgress = model.GenerateResponseWithProgressAsync(prompt);
languageModelRepsonseWithProgress.Progress = (_, generationProgress) =>
{
    progress.Report(generationProgress);
};
string response = (await languageModelResponseWithProgress).Response;
