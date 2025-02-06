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

namespace WindowsCopilotRuntimeSample.Models;

internal class LanguageModelModel : IModelManager
{
    private LanguageModel? _session;

    public async Task CreateModelSessionWithProgress(IProgress<PackageDeploymentProgress> progress,
                                                            CancellationToken cancellationToken = default)
    {
        if (!LanguageModel.IsAvailable())
        {
            var languageModelDeploymentOperation = LanguageModel.MakeAvailableAsync();
            languageModelDeploymentOperation.Progress = (_, packageDeploymentProgress) =>
            {
                progress.Report(packageDeploymentProgress);
            };
            using var _ = cancellationToken.Register(() => languageModelDeploymentOperation.Cancel());
            await languageModelDeploymentOperation;
        }
        else
        {
            progress.Report(new PackageDeploymentProgress(PackageDeploymentProgressStatus.CompletedSuccess, 100.0));
        }
        _session = await LanguageModel.CreateAsync();
    }

    private LanguageModel Session => _session ?? throw new InvalidOperationException("Language Model session was not created yet");

    public async Task<string> GenerateResponseAsync(string prompt, CancellationToken cancellationToken = default)
    {
        var response = await Session.GenerateResponseAsync(prompt);
        return response.Response;
    }

    public IAsyncOperationWithProgress<LanguageModelResponse, string> 
        GenerateResponseWithProgressAsync(string prompt, CancellationToken cancellationToken = default)
    {
        var response = Session.GenerateResponseWithProgressAsync(prompt);
        return new AsyncOperationWithProgressAdapter<LanguageModelResponse, string, LanguageModelResponse, string>(
            response,
            result => result /* LanguageModelResponse */,
            progress => progress
        );
    }

    public IAsyncOperationWithProgress<LanguageModelResponse, string>
        GenerateResponseWithOptionsAndProgressAsync(string prompt, CancellationToken cancellationToken = default)
    {
        var response = Session.GenerateResponseWithProgressAsync(prompt);
        return new AsyncOperationWithProgressAdapter<LanguageModelResponse, string, LanguageModelResponse, string>(
            response,
            result => result /* LanguageModelResponse */,
            progress => progress
        );
    }

    public async Task<IReadOnlyList<EmbeddingVector>> GenerateEmbeddingVectorAsync(string prompt, CancellationToken cancellationToken = default)
    {
        var response = await Session.GenerateEmbeddingVectorAsync(prompt);
        return response;
    }

    public IAsyncOperationWithProgress<LanguageModelResponse, string> 
        GenerateResponseWithOptionsAndProgressAsync(string prompt, LanguageModelOptions languageModelOptions,
        ContentFilterOptions? contentFilterOptions, CancellationToken cancellationToken = default)
    {
        IAsyncOperationWithProgress<LanguageModelResponse, string> response;
        if (contentFilterOptions != null)
        {
            response = Session.GenerateResponseWithProgressAsync(languageModelOptions, prompt, contentFilterOptions);
        }
        else
        {
            // in case content filter option is not provided, default value is picked up for content filtering
            response = Session.GenerateResponseWithProgressAsync(languageModelOptions, prompt);
        }
        return new AsyncOperationWithProgressAdapter<LanguageModelResponse, string, LanguageModelResponse, string>(
            response,
            result => result /* LanguageModelResponse */,
            progress => progress
        );
    }

    public IAsyncOperationWithProgress<LanguageModelResponse, string> 
        GenerateResponseWithContextAsync(string prompt, string context, CancellationToken cancellationToken = default)
    {
        var languageModelOptions = new LanguageModelOptions();
        var contentFilterOptions = new ContentFilterOptions();

        var languageModelContext = Session.CreateContext(context, contentFilterOptions);

        var response = Session.GenerateResponseWithProgressAsync(languageModelOptions, prompt, contentFilterOptions, languageModelContext);

        return new AsyncOperationWithProgressAdapter<LanguageModelResponse, string, LanguageModelResponse, string>(
            response,
            result => result /* LanguageModelResponse */,
            progress => progress
        );
    }
}
