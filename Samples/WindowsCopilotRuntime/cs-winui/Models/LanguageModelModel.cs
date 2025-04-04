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
    }

    private LanguageModel Session => _session ?? throw new InvalidOperationException("Language Model session was not created yet");

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
}
