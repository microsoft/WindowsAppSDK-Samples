// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using WindowsCopilotRuntimeSample.Models.Contracts;
using WindowsCopilotRuntimeSample.Util;
using Microsoft.Windows.AI.Generative;
using Microsoft.Windows.Management.Deployment;
using Microsoft.Windows.SemanticSearch;
using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using Windows.Foundation;
using Microsoft.Windows.AI.ContentModeration;
using System.ComponentModel.DataAnnotations;
using Microsoft.Windows.AI;

namespace WindowsCopilotRuntimeSample.Models;

internal class LanguageModelModel : IModelManager
{
    private LanguageModel? _session;
    private TextSummarizer? _sessionTextSummarize;
    private TextRewriter? _sessionTextRewrite;
    private TextToTableConverter? _sessionTextToTable;

    public async Task CreateModelSessionWithProgress(IProgress<double> progress,
                                                            CancellationToken cancellationToken = default)
    {
        if (LanguageModel.GetReadyState() == AIFeatureReadyState.EnsureNeeded)
        {
            var languageModelDeploymentOperation = LanguageModel.EnsureReadyAsync();
            languageModelDeploymentOperation.Progress = (_, modelDeploymentProgress) =>
            {
                progress.Report(modelDeploymentProgress);
            };
            using var _ = cancellationToken.Register(() => languageModelDeploymentOperation.Cancel());
            await languageModelDeploymentOperation;
        }
        else
        {
            progress.Report(100.0);
        }
        _session = await LanguageModel.CreateAsync();
        _sessionTextSummarize = new TextSummarizer(_session);
        _sessionTextRewrite = new TextRewriter(_session);
        _sessionTextToTable = new TextToTableConverter(_session);
    }

    private LanguageModel Session => _session ?? throw new InvalidOperationException("Language Model session was not created yet");
    private TextSummarizer SessionTextSummarize => _sessionTextSummarize ?? throw new InvalidOperationException("Language Model session was not created yet");
    private TextRewriter SessionTextRewrite => _sessionTextRewrite ?? throw new InvalidOperationException("Language Model session was not created yet");
    private TextToTableConverter SessionTextToTable => _sessionTextToTable ?? throw new InvalidOperationException("Language Model session was not created yet");

    public IAsyncOperationWithProgress<LanguageModelResponseResult, string> 
        GenerateResponseWithProgressAsync(string prompt, CancellationToken cancellationToken = default)
    {
        var response = Session.GenerateResponseAsync(prompt);
        return new AsyncOperationWithProgressAdapter<LanguageModelResponseResult, string, LanguageModelResponseResult, string>(
            response,
            result => result /* LanguageModelResponse */,
            progress => progress
        );
    }

    public IAsyncOperationWithProgress<LanguageModelResponseResult, string>
        GenerateResponseWithOptionsAndProgressAsync(string prompt, CancellationToken cancellationToken = default)
    {
        var response = Session.GenerateResponseAsync(prompt);
        return new AsyncOperationWithProgressAdapter<LanguageModelResponseResult, string, LanguageModelResponseResult, string>(
            response,
            result => result /* LanguageModelResponse */,
            progress => progress
        );
    }

    public LanguageModelEmbeddingVectorResult GenerateEmbeddingVectors(string prompt, CancellationToken cancellationToken = default)
    {
        var response = Session.GenerateEmbeddingVectors(prompt);
        return response;
    }

    public IAsyncOperationWithProgress<LanguageModelResponseResult, string> 
        GenerateResponseWithOptionsAndProgressAsync(string prompt, LanguageModelOptions languageModelOptions,
        ContentFilterOptions? contentFilterOptions, CancellationToken cancellationToken = default)
    {
        IAsyncOperationWithProgress<LanguageModelResponseResult, string> response;
        if (contentFilterOptions != null)
        {
            languageModelOptions.ContentFilterOptions = contentFilterOptions;
        }
        
        response = Session.GenerateResponseAsync(prompt, languageModelOptions);
        
        return new AsyncOperationWithProgressAdapter<LanguageModelResponseResult, string, LanguageModelResponseResult, string>(
            response,
            result => result /* LanguageModelResponseResult */,
            progress => progress
        );
    }

    public IAsyncOperationWithProgress<LanguageModelResponseResult, string> 
        GenerateResponseWithContextAsync(string prompt, string contextPrompt, CancellationToken cancellationToken = default)
    {
        var languageModelOptions = new LanguageModelOptions();
        var contentFilterOptions = new ContentFilterOptions();
        var languageModelContext = Session.CreateContext(contextPrompt, contentFilterOptions);

        var response = Session.GenerateResponseAsync(languageModelContext, prompt, languageModelOptions);

        return new AsyncOperationWithProgressAdapter<LanguageModelResponseResult, string, LanguageModelResponseResult, string>(
            response,
            result => result /* LanguageModelResponseResult */,
            progress => progress
        );
    }

    public IAsyncOperationWithProgress<LanguageModelResponseResult, string>
    GenerateResponseTextIntelligenceSummarizeWithProgressAsync(string prompt)
    {
        IAsyncOperationWithProgress<LanguageModelResponseResult, string> response;

        response = SessionTextSummarize.SummarizeAsync(prompt);

        return new AsyncOperationWithProgressAdapter<LanguageModelResponseResult, string, LanguageModelResponseResult, string>(
            response,
            result => result /* LanguageModelResponseResult */,
            progress => progress
        );
    }

    public IAsyncOperationWithProgress<LanguageModelResponseResult, string>
    GenerateResponseTextIntelligenceRewriteWithProgressAsync(string prompt)
    {
        IAsyncOperationWithProgress<LanguageModelResponseResult, string> response;

        response = SessionTextRewrite.RewriteAsync(prompt);

        return new AsyncOperationWithProgressAdapter<LanguageModelResponseResult, string, LanguageModelResponseResult, string>(
            response,
            result => result /* LanguageModelResponseResult */,
            progress => progress
        );
    }

    public string 
    GenerateResponseTextIntelligenceTextToTableAsync(string prompt)
    {
        IAsyncOperationWithProgress<TextToTableResponseResult, string> response;
        response = SessionTextToTable.ConvertAsync(prompt);

        string result = string.Empty;
        var rows = response.Get().GetRows();

        if (rows != null)
        {
            foreach (var row in rows)
            {
                result += string.Join(',', row.GetColumns()) + Environment.NewLine;
            }
        }
        else
        {
            result = "No table was generated from the prompt. Please try different prompt";
        }

        return result;
    }
}
