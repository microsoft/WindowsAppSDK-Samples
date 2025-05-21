IProgress<string> progress;
using LanguageModel languageModel = await LanguageModel.CreateAsync();
var textRewriter = new TextRewriter(languageModel);

var textRewriterResponseWithProgress = textRewriter.RewriteAsync(prompt);

string progressText = string.Empty;
textRewriterResponseWithProgress.Progress = (_, generationProgress) =>
{
    progress.Report(generationProgress);
};
var response = await textRewriterResponseWithProgress;
