IProgress<string> progress;
using LanguageModel languageModel = await LanguageModel.CreateAsync();
var textSummarizer = new TextSummarizer(languageModel);

var textSummarizerResponseWithProgress = textSummarizer.SummarizeAsync(prompt);

string progressText = string.Empty;
textSummarizerResponseWithProgress.Progress = (_, generationProgress) =>
{
    progress.Report(generationProgress);
};
var response = await textSummarizerResponseWithProgress;
